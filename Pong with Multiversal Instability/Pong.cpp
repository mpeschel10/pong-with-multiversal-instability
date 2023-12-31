#include <GL/glew.h> // Important that this is included before glut.h
#include <GL/glut.h>
#include <irrKlang.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <chrono>
#include <climits>
#include <random>
float randomFloat() { return rand() / float(RAND_MAX); }
float randomFloat(float range) {
    float f = randomFloat();
    float result = f * range;
    //std::cout << "Generated result " << result << " | Range: " << range << std::endl;
    return result;
}

#include "Point.cpp"
#include "Peg.cpp"
#include "LineSegment.cpp"
#include "Path.cpp"
#include "Texture.cpp"
#include "Paddle.cpp"
#include "Slider.cpp"

#define PI 3.14159265
#define TARGET_FPS 60.0
static int frameCount = 0;
static long lastSecond = -1;
static long lastFrameTime = -1; // In milliseconds
static long thisFrameTime; // In milliseconds
static float deltaTime; // In seconds

using namespace std;
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();

const int windowHeight = 630;
const int windowWidth = 1000;
const struct Point windowSize = { float(windowWidth), float(windowHeight) };
const int millisecondsPerFrame = int(1000.0 / TARGET_FPS);

TexturedRectangle title("textures/Title.png");
TexturedRectangle background_title("textures/bg.png"); // Not needed anymore unless we want a settings option to not have the spinning screen
TexturedRectangle background_swirl("textures/swirl.png");
TexturedRectangle ping_pong_texture("textures/ping-pong.png"), baseball_texture("textures/baseball.png"),
basketball_texture("textures/basketball.png"), orange_texture("textures/orange.png");
TexturedRectangle barber_texture("textures/barber.png"), fries_texture("textures/fries.png"),
paddle_texture("textures/paddle.png"), sword_texture("textures/sword.png");
TexturedRectangle board("textures/board.png");

struct Paddle p1, p2;
const float paddleSpeed = 300; // in pixels per second

TexturedRectangle* paddle_textures[] = {
    &barber_texture, &fries_texture, &paddle_texture, &sword_texture,
};
int activePaddleOneTexture = -1;
int activePaddleTwoTexture = -1;

float paddle_colors[10][3] = { {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                                  { 0.0f, 0.0f, 1.0f }, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}, {0.625f, 0.375f, 0.0f} };
int activePaddleOneColor = -1;
int activePaddleTwoColor = -1;

// Ball
float ballX, ballY, ballSpeedX, ballSpeedY;
const float ballSpeed = 240; // in pixels per second
int speedUp = 0;
int boost = 0;
const float ballDiameter = 20;
const float ballRadius = ballDiameter / 2;
int cooldown;
int isAI = 2;

// Note: The order of ball_textures is important!
// Must match up with order of menu entries in initTextureMenu.
TexturedRectangle* ball_textures[] = {
    &ping_pong_texture, &baseball_texture, &basketball_texture, &orange_texture
};
static int activeBallTexture = -1;

// Scoring
int player1Score = 0;
int player2Score = 0;

// - Function Initialization -
// Primarily used by MenuScreens.cpp to prevent errors
void idle();
void renderText(const string& text, float x, float y);
void renderText(const string& text, float x, float y, float r, float g, float b);
void renderTextWarpable(const string& text, float x, float y);
void setGameMode(int mode);
void init();
void display();
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
float squareDistance(int x, int y, Point p);

// Modifiers
int randAngle = rand() % 361;
float rotateAngle = 0;
float xScale = 1.0;
float yScale = 1.0;
float scaleAngle = 0.0;
float windowPosX = 0.0;
float windowPosY = 0.0;
float xBefore = 0.0;
float yBefore = 0.0;
float rgb[] = { 100, 100, 100 };
float goalRGB[3];
vector<vector<float>> previousBallPositions;
float flashAlpha;
bool super = false;
const float pegSize = 20;
const int pegCount = 20;
Peg pegs[pegCount];

// Centralize state management in setGameMode function
// At the moment, this only controls setting state for the timer function, since that's what Mark is working on
#define MODE_TITLE 2
#define MODE_VS_PLAYER 0
#define MODE_VS_AI 1
#define MODE_WIN_PAUSE 3
#define MODE_PAUSE 4
#define MODE_AI_VS_AI 5
#define MODE_SETTINGS 6
#define MODE_MODIF_SETTINGS 7
static int game_mode = MODE_TITLE;

// Modifier variable
#define MODIF_NONE 0
#define MODIF_ROTATE 1
#define MODIF_TILT 2
#define MODIF_SUPER 3
#define MODIF_SCALE 4
#define MODIF_WOOZY 5
#define MODIF_DIZZY 6
#define MODIF_STABLE 7
#define MODIF_BEZIER_FREE 8
#define MODIF_PONGLE 9
#define MODIF_GAMER 10
#define MODIF_PHONG 11
#define MODIF_SNONG 12
#define MODIF_OMNI 13
#define MODIF_FLASH 14
static int modifier = MODIF_NONE;
const int numModifiers = 15;
string descriptions[numModifiers] = { "No Modifier", "Rotate Pong", "Tilt Pong", "SUPERPONG", "Scale Pong", "Woozy Pong",
                                      "Dizzy Pong", "Stable Pong", "Bezier Pong", "Pongle", "Gamer Pong", "Phong", "Snong", "OMNIPONG", "Flash Pong"};
float modifProbs[numModifiers] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

// Track what keys are down for smooth updates.
bool keyboardDown[255] = {}; // To check for 'a' key, do keyboardDown['a']. Single quote characters are ints in C++
bool specialDown[255] = {}; // To check for left key, do specialDown[GLUT_KEY_LEFT]
Point mousePosition = {};
Point* draggingPoint = NULL;
Point draggingPointOffset = {};

#include "ModifMenuOption.cpp"
#include "MenuScreens.cpp" // Title and Settings screen

void renderText(const string& text, float x, float y) {
    // Professor Reza's Render Text Function
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight); // Modified for the window size of our application

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f); // Text Color: White

    if (modifier == MODIF_GAMER) {
        glColor3f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100);
    }

    glRasterPos2f(x, y);

    // Loop through the characters in the text and render them one by one
    for (const char& character : text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, character);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void renderText(const string& text, float x, float y, float r, float g, float b) {
    // Professor Reza's Render Text Function with added text color specifications
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight); // Modified for the window size of our application

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(r, g, b); // Normal Text Color: White

    if (modifier == MODIF_GAMER) {
        glColor3f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100);
    }

    glRasterPos2f(x, y);

    // Loop through the characters in the text and render them one by one
    for (const char& character : text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, character);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void renderTextWarpable(const string& text, float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f); // Text Color: White

    if (modifier == MODIF_GAMER) {
        glColor3f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100);
    }

    glRasterPos2f(x, y);

    // Loop through the characters in the text and render them one by one
    for (const char& character : text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, character);
    }
}

long now() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();


    if (modifier == MODIF_TILT) {
        glTranslatef((windowWidth / 2), (windowHeight / 2), 0);
        glRotatef(randAngle, 0.0, 0.0, 1.0);
        glTranslatef(-(windowWidth / 2), -(windowHeight / 2), 0);
        glScalef(0.5, 0.5, 1);
        glTranslatef(windowWidth / 2, windowHeight / 2, 0);
    }
    else if (modifier == MODIF_ROTATE || modifier == MODIF_DIZZY || modifier == MODIF_OMNI) {
        glTranslatef((windowWidth / 2), (windowHeight / 2), 0);
        glRotatef(rotateAngle, 0.0, 0.0, 1.0);
        glTranslatef(-(windowWidth / 2), -(windowHeight / 2), 0);
        glScalef(0.5, 0.5, 1);
        glTranslatef(windowWidth / 2, windowHeight / 2, 0);
    }
    if (modifier == MODIF_SCALE || modifier == MODIF_WOOZY || modifier == MODIF_DIZZY || modifier == MODIF_OMNI) {
        float xOffset = ((windowWidth * (1 - xScale)) / 2.0);
        float yOffset = (((windowHeight - 30) * (1 - yScale)) / 2.0);
        glTranslatef(xOffset, yOffset, 0);
        glScalef(xScale, yScale, 1);
    }
    if (modifier == MODIF_GAMER || modifier == MODIF_OMNI) {
        glColor3f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100);
    }

    board.display();

    if (game_mode != MODE_WIN_PAUSE) {
        glColor3f(0.5f, 0.5f, 0.5f);

        if (modifier == MODIF_GAMER || modifier == MODIF_OMNI) {
            glColor3f(rgb[0] / 200, rgb[1] / 200, rgb[2] / 200);
        }

        glRectf((windowWidth / 2.0) - 5, ((windowHeight - 30) / 2.0) - 1, (windowWidth / 2.0) + 5, ((windowHeight - 30) / 2.0) + 1);
        glRectf((windowWidth / 2.0) - 1, 1, (windowWidth / 2.0) + 1, windowHeight - 31);
        glColor3f(1.0f, 1.0f, 1.0f);

        if (modifier == MODIF_GAMER || modifier == MODIF_OMNI) {
            glColor3f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100);
        }
    }
    
    glBegin(GL_LINES); // Draw Play Area

    glVertex2i(1, 0);
    glVertex2i(windowWidth, 0); // Originally Y1: 1 | X2: windowWidth-1

    glVertex2i(windowWidth, 0);
    glVertex2i(windowWidth, windowHeight - 31);

    glVertex2i(windowWidth, windowHeight - 31);
    glVertex2i(1, windowHeight - 31);

    glVertex2i(1, windowHeight - 31);
    glVertex2i(1, 0);

    glEnd();

    // Paddles

    if (activePaddleOneTexture == -1) {
        glColor3f(paddle_colors[activePaddleOneColor + 1][0], paddle_colors[activePaddleOneColor + 1][1], paddle_colors[activePaddleOneColor + 1][2]);
        paddleDraw(p1);
    }
    else {
        glColor3f(paddle_colors[activePaddleOneColor + 1][0], paddle_colors[activePaddleOneColor + 1][1], paddle_colors[activePaddleOneColor + 1][2]);
        paddleDraw(p1, paddle_textures[activePaddleOneTexture]);
    }

    if (activePaddleTwoTexture == -1) {
        glColor3f(paddle_colors[activePaddleTwoColor + 1][0], paddle_colors[activePaddleTwoColor + 1][1], paddle_colors[activePaddleTwoColor + 1][2]);
        paddleDraw(p2);
    }
    else {
        glColor3f(paddle_colors[activePaddleTwoColor + 1][0], paddle_colors[activePaddleTwoColor + 1][1], paddle_colors[activePaddleTwoColor + 1][2]);
        paddleDraw(p2, paddle_textures[activePaddleTwoTexture]);
    }

    // Paddle paths
    if (modifier == MODIF_BEZIER_FREE) {
        bezierDraw(p1.path);
        bezierDraw(p2.path);
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    if (modifier == MODIF_GAMER || modifier == MODIF_OMNI) {
        glColor3f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100);
    }

    // Pongle pegs
    if (modifier == MODIF_PONGLE) {
        for (int i = 0; i < pegCount; i++) {
            const Peg p = pegs[i];
            p.display(false); // false means don't color it yellow
        }
    }

    // Ball
    if (activeBallTexture == -1) {
        glRectf(ballX - ballRadius, ballY - ballRadius, ballX + ballRadius, ballY + ballRadius);
    }
    else {
        ball_textures[activeBallTexture]->centerxy(ballX, ballY);
        ball_textures[activeBallTexture]->display();
    }

    if (modifier == MODIF_SNONG || modifier == MODIF_OMNI) {
        //float alpha = 1.0f;
        for (int i = 0; i < previousBallPositions.size(); i++) {
            //glColor4f(1.0f, 1.0f, 1.0f, alpha);
            if (activeBallTexture == -1) {
                glRectf(previousBallPositions[i][0] - ballRadius, previousBallPositions[i][1] - ballRadius,
                    previousBallPositions[i][0] + ballRadius, previousBallPositions[i][1] + ballRadius);
            }
            else {
                ball_textures[activeBallTexture]->centerxy(previousBallPositions[i][0], previousBallPositions[i][1]);
                ball_textures[activeBallTexture]->display();
            }
            //alpha -= 0.2f;
        }
        if (previousBallPositions.size() >= speedUp + 1) {
            previousBallPositions.resize(speedUp + 1);
        }
    }

    if (modifier == MODIF_FLASH) {
        glColor4f(0.0f, 0.0f, 0.0f, flashAlpha);
        glRectf(1, 0, windowWidth - 1, windowHeight - 31);
    }

    // Render Score
    string score = to_string(player1Score) + " | " + to_string(player2Score);
    int scoreLength = to_string(player1Score).length();
    renderText(score, 491.0 - scoreLength * 12.0, 606.0);

    // Render Active Modifier
    string activeModifier = descriptions[modifier];
    scoreLength = activeModifier.length();
    renderText(activeModifier, 10, 606);

    frameCount++;
    if (now() - lastSecond >= 1000) {
        // printf("FPS: %d  \r", frameCount);
        std::cout.flush();
        frameCount = 0;
        lastSecond = now();
    }

    if (game_mode == MODE_PAUSE) {
        glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

        if (modifier == MODIF_GAMER || modifier == MODIF_OMNI) {
            glColor4f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100, 0.5f);
        }
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glRectf((windowWidth / 2.0) - 55, (windowHeight / 2.0) + 40, (windowWidth / 2.0) + 55, (windowHeight / 2.0) - 25);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        renderText("PAUSED", (windowWidth / 2.0) - 47, (windowHeight / 2.0));
    }

    if (game_mode == MODE_WIN_PAUSE) {

        glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

        if (modifier == MODIF_GAMER || modifier == MODIF_OMNI) {
            glColor4f(rgb[0] / 100, rgb[1] / 100, rgb[2] / 100, 0.5f);
        }

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glRectf((windowWidth / 2.0) - 55, (windowHeight / 2.0) + 40, (windowWidth / 2.0) + 55, (windowHeight / 2.0) - 25);

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        if (cooldown > 120) {
            renderText("3", (windowWidth / 2.0) - 6, (windowHeight / 2.0));
        }
        else if (cooldown > 60) {
            renderText("2", (windowWidth / 2.0) - 6, (windowHeight / 2.0));
        }
        else if (cooldown > 0) {
            renderText("1", (windowWidth / 2.0) - 6, (windowHeight / 2.0));
        }
    }

    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glutReshapeWindow(windowWidth, windowHeight);
    lastFrameTime = -1;
}

void reset() {
    float windowCenterY = (windowHeight - 30) / 2.0;
    // Divide by TARGET_FPS to approximate multiplying by deltaTime.
    // This is bad, but the Bezeir2 class atm doesn't admit a better solution.
    p1.targetSpeed = paddleSpeed / TARGET_FPS; p1.t = 0.5;
    p2.targetSpeed = paddleSpeed / TARGET_FPS; p2.t = 0.5;

    paddleLeftX(p1, 10.0);
    paddleCenterY(p1, windowCenterY);
    paddleVerticalPath(p1, 15.0, 1, windowHeight - 31);

    paddleRightX(p2, windowWidth - 10.0);
    paddleCenterY(p2, windowCenterY);
    paddleVerticalPath(p2, windowWidth - 15.0, 1, windowHeight - 31);

    ballX = windowWidth / 2.0;
    ballY = windowCenterY;
    speedUp = 0;

    float startAngles[] = { 15.0, 105.0, 195.0, 285.0 };
    float degrees = startAngles[rand() % 4] + rand() % 60;
    float radians = degrees * PI / 180.0;

    ballSpeedX = ballSpeed * cos(radians);
    ballSpeedY = ballSpeed * sin(radians);

    lastFrameTime = -1;
    lastSecond = -1;
    glutPositionWindow(200, 100);
}

float squareDistance(int x, int y, Point p) {
    return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
}

Point* checkClickedPoint(float radius, int x, int y, Paddle& paddle) {
    if (squareDistance(x, y, paddle.path.p0) <= radius) return &paddle.path.p0;
    if (squareDistance(x, y, paddle.path.p1) <= radius) return &paddle.path.p1;
    if (squareDistance(x, y, paddle.path.p2) <= radius) return &paddle.path.p2;
    return NULL;
}

void onMouse(int button, int state, int x, int y) {
    y = windowHeight - y;
    if (modifier == MODIF_BEZIER_FREE) {
        if (button != GLUT_LEFT_BUTTON) return;

        if (state == GLUT_UP) {
            draggingPoint = NULL;
            return;
        }
        if (state != GLUT_DOWN) return;

        // std::cout << "Mouse click at " << x << ", " << y << std::endl;
        float clickRadius = 2500;
        Point* clickedPoint = checkClickedPoint(clickRadius, x, y, p1);
        if (clickedPoint == NULL)
            clickedPoint = checkClickedPoint(clickRadius, x, y, p2);

        if (clickedPoint == NULL) return;

        // std::cout << "Clicked point" << std::endl;
        draggingPoint = clickedPoint;
        draggingPointOffset = *clickedPoint - Point{ float(x), float(y) };
    }
}

void onMotion(int x, int y) { mousePosition = Point{ float(x), float(windowHeight - y) }; }
void onPassiveMotion(int x, int y) { onMotion(x, y); }

void setGameMode(int mode) {
    bool modeIsValid = true;
    glutIdleFunc(NULL);
    switch (mode) {
    case MODE_TITLE:
        // Functionality for resetting to the title screen here
        player1Score = 0;
        player2Score = 0;
        titleInit();
        glutDisplayFunc(titleDisplay);
        glutKeyboardFunc(titleKeyboard);
        glutMouseFunc(titleMouse);
        glutMotionFunc(NULL);
        glutPassiveMotionFunc(NULL);
        glutSpecialFunc(NULL);
        glutIdleFunc(titleIdle);
        break;
    case MODE_VS_PLAYER:
        glutMotionFunc(onMotion);
        glutPassiveMotionFunc(onPassiveMotion);
        glutMouseFunc(onMouse);
        break;
    case MODE_VS_AI:
        glutMotionFunc(onMotion);
        glutPassiveMotionFunc(onPassiveMotion);
        glutMouseFunc(onMouse);
        break;
    case MODE_AI_VS_AI:
        glutMotionFunc(onMotion);
        glutPassiveMotionFunc(onPassiveMotion);
        glutMouseFunc(onMouse);
        break;
    case MODE_PAUSE:
        lastFrameTime = -1;
        break;
    case MODE_WIN_PAUSE:
        cooldown = 180;
        break;
    case MODE_SETTINGS:
        settingsInit();
        glutDisplayFunc(settingsDisplay);
        glutKeyboardFunc(settingsKeyboard);
        glutMouseFunc(settingsMouse);
        glutSpecialFunc(NULL);
        break;
    case MODE_MODIF_SETTINGS:
        modifSettingsInit();
        glutDisplayFunc(modifSettingsDisplay);
        glutKeyboardFunc(modifSettingsKeyboard);
        glutMouseFunc(modifSettingsMouse);
        glutSpecialFunc(NULL);
        break;
    default:
        std::cerr << "Warning: setGameMode was called with invalid mode " << mode << std::endl;
        modeIsValid = false;
        break;
    }
    if (modeIsValid)
    {
        game_mode = mode;
    }
}

void updateDrag() {
    if (draggingPoint == NULL) return;
    *draggingPoint = mousePosition + draggingPointOffset;
    paddleUpdate(p1);
    paddleUpdate(p2);
    glutPostRedisplay();
}

void updatePaddles() {

    if (game_mode == MODE_VS_PLAYER || game_mode == MODE_VS_AI) {
        if (keyboardDown['w']) {
            if (p1.y1 <= windowHeight - 31) {
                paddleMoveT(p1, p1.tOffset);

                if (modifier == MODIF_SUPER || modifier == MODIF_OMNI) super = true;
            }
        }
        if (keyboardDown['s']) {
            if (p1.y2 >= 1) {
                paddleMoveT(p1, -p1.tOffset);

                if (modifier == MODIF_SUPER || modifier == MODIF_OMNI) super = true;
            }
        }
    }

    if (game_mode == MODE_VS_PLAYER) {
        if (specialDown[GLUT_KEY_UP]) {
            if (p2.y1 <= windowHeight - 31) {
                paddleMoveT(p2, p2.tOffset);

                if (modifier == MODIF_SUPER || modifier == MODIF_OMNI) super = true;
            }
        }
        if (specialDown[GLUT_KEY_DOWN]) {
            if (p2.y2 >= 1) {
                paddleMoveT(p2, -p2.tOffset);

                if (modifier == MODIF_SUPER || modifier == MODIF_OMNI) super = true;
            }
        }
    }
}

void updateBall() {
    bool bounceWeirdness = false;

    if (modifier == MODIF_PONGLE) {
        struct Point ballVelocity = Point{ ballSpeedX, ballSpeedY };
        float oldSpeed = magnitude(ballVelocity); // May cause drift due to converting to and from lengths and offsets.

        // I should rename the LineSegment velocity to reflect the fact that it's a vector with a location...
        LineSegment velocity(Point{ ballX, ballY }, ballVelocity * deltaTime);

        // Don't just check ahead from our centerpoint; check ahead from the furthest point on our surface.
        // Hopefully, we can bounce off the peg without visually intersecting it.
        velocity.length += ballRadius;

        for (const Peg& peg : pegs) {
            struct Point point = velocity.intersection(peg);
            if (point == NO_INTERSECTION) continue;

            // std::cout << "Bounce peg " << peg << std::endl;

            struct Point bouncePoint = point;
            float distanceToBouncePoint = distance(velocity.start, bouncePoint);
            // Undo adding ballRadius to our speed.
            distanceToBouncePoint -= ballRadius;
            velocity.length -= ballRadius;
            struct Point reflectionPoint = velocity.point(distanceToBouncePoint); // Where our center will be when we bounce

            float remainingLength = velocity.length - distanceToBouncePoint;
            LineSegment normal = peg.normal(bouncePoint);
            struct Point reflectedDirection = reflect(-velocity.direction, normal.direction);
            LineSegment reflectedVelocity = LineSegment{ reflectionPoint, reflectedDirection, remainingLength };

            struct Point positionAfterBounce = reflectedVelocity.getEnd();
            ballSpeedX = reflectedVelocity.direction.x * oldSpeed * 1.05;
            ballSpeedY = reflectedVelocity.direction.y * oldSpeed * 1.05;
            ballX = positionAfterBounce.x;
            ballY = positionAfterBounce.y;
            bounceWeirdness = true;
            break;
        }
    }

    if (ballY + (ballDiameter / 2.0) >= windowHeight - 31) {

        ballSpeedY = -abs(ballSpeedY);

        if (modifier == MODIF_PHONG) {
            ballSpeedY *= rand() % 2 == 0 ? 0.65 : 1.35;
        }

        speedUp += 1;
        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }

        SoundEngine->play2D("audio/bounce.wav");
    }
    else if (ballY - (ballDiameter / 2.0) <= 0) {

        ballSpeedY = abs(ballSpeedY);

        if (modifier == MODIF_PHONG) {
            ballSpeedY *= rand() % 2 == 0 ? 0.65 : 1.35;
        }

        speedUp += 1;
        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }

        SoundEngine->play2D("audio/bounce.wav");
    }

    if (paddleContains(p1, ballX, ballY)) {

        ballX = p1.x2;
        ballSpeedX = 0 - ballSpeedX;
        speedUp += 1;

        if (modifier == MODIF_PHONG) {
            ballSpeedX *= rand() % 2 == 0 ? 0.65 : 1.35;
        }

        if (keyboardDown['e']) {
            ballSpeedX *= 2;
            ballSpeedY *= 2;
            boost = 50;
        }

        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }
        SoundEngine->play2D("audio/bounce.wav");
    }
    else if (paddleContains(p2, ballX, ballY)) {

        ballX = p2.x1;
        ballSpeedX = 0 - ballSpeedX;
        speedUp += 1;

        if (modifier == MODIF_PHONG) {
            ballSpeedX *= rand() % 2 == 0 ? 0.65 : 1.35;
        }

        if (specialDown[GLUT_KEY_LEFT]) {
            ballSpeedX *= 2;
            ballSpeedY *= 2;
            boost = 50;
        }

        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }
        SoundEngine->play2D("audio/bounce.wav");
    }

    if ((ballX + (ballDiameter / 2.0) >= windowWidth)) {
        //cout << "Player 1 wins the game" << endl;
        player1Score += 1;
        //reset();
        setGameMode(MODE_WIN_PAUSE);
        glutPostRedisplay();

        SoundEngine->play2D("audio/loss.wav");
        return;
    }
    else if ((ballX - (ballDiameter / 2.0) <= 1)) {
        //cout << "Player 2 wins the game" << endl;
        player2Score += 1;
        //reset();
        setGameMode(MODE_WIN_PAUSE);
        glutPostRedisplay();

        SoundEngine->play2D("audio/loss.wav");
        return;
    }

    // If bounce off peg, handle the actual movement earlier in the method.
    if (!bounceWeirdness) {
        xBefore = ballX;
        yBefore = ballY;
        ballX += ballSpeedX * deltaTime;
        ballY += ballSpeedY * deltaTime;
    }

    if (boost > 0) {
        boost--;

        if (boost == 0) {
            ballSpeedX /= 2;
            ballSpeedY /= 2;
        }
    }
}

void updateAI() {

    if (isAI == 1) {
        int rdm = (rand() % 3); // 2/3  chance for the AI to move
        if (isAI && rdm >= 1 && ballX > (windowWidth / 2.0) && ballSpeedX > 0) { // AI
            if (ballY >= (p2.y1 - 20) && p2.y1 <= windowHeight - 31) {
                paddleMoveT(p2, p2.tOffset);
            }
            else if (ballY <= (p2.y2 + 20) && p2.y2 >= 1) {
                paddleMoveT(p2, -p2.tOffset);
            }
        }
    }
    else if (isAI == 2) {

        if (ballX > (windowWidth / 2.0) && ballSpeedX > 0) { // AI
            if (ballY >= (p2.y1 - 45) && p2.y1 <= windowHeight - 31) {
                paddleMoveT(p2, p2.tOffset);
            }
            else if (ballY <= (p2.y2 + 45) && p2.y2 >= 1) {
                paddleMoveT(p2, -p2.tOffset);
            }
        }

        if (ballX < (windowWidth / 2.0) && ballSpeedX < 0) {
            if (ballY >= (p1.y1 - 45) && p1.y1 <= windowHeight - 31) {
                paddleMoveT(p1, p1.tOffset);
            }
            else if (ballY <= (p1.y2 + 45) && p1.y2 >= 1) {
                paddleMoveT(p1, -p1.tOffset);
            }
        }

    }
}

void updateModifier() {
    bool same;
    vector<float> vec;
    switch (modifier) {
    case MODIF_ROTATE:
        rotateAngle += 0.1;
        break;
    case MODIF_WOOZY:
        scaleAngle += 0.025;
        if (scaleAngle >= 360)
            scaleAngle = 0.0;
        xScale = (cos(scaleAngle) / 4.0) + 0.75;
        yScale = (sin(scaleAngle) / 4.0) + 0.75;
        break;
    case MODIF_DIZZY:
        rotateAngle += 0.1;
        scaleAngle += 0.025;
        if (scaleAngle >= 360)
            scaleAngle = 0.0;
        xScale = (cos(scaleAngle) / 4.0) + 0.75;
        yScale = (sin(scaleAngle) / 4.0) + 0.75;
        break;
    case MODIF_STABLE:
        windowPosX -= (ballX - xBefore);
        windowPosY += (ballY - yBefore);
        glutPositionWindow(windowPosX, windowPosY); // (?)
        break;
    case MODIF_GAMER:
        same = true;
        for (int i = 0; i < 2; i++) {
            if (goalRGB[i] != rgb[i]) {
                same = false;
            }
        }
        if (same) {
            int randVal;
            for (int i = 0; i < 3; i++) {
                randVal = (rand() % 50) + 50;
                goalRGB[i] = randVal;
            }
        }
        else {
            int randVal = rand() % 3;
            if (rgb[randVal] > goalRGB[randVal]) {
                rgb[randVal] -= 1;
            }
            else if (rgb[randVal] < goalRGB[randVal]) {
                rgb[randVal] += 1;
            }
        }
        break;
    case MODIF_SNONG:
        vec.push_back(ballX);
        vec.push_back(ballY);
        previousBallPositions.insert(previousBallPositions.begin(), vec);
        break;
    case MODIF_FLASH:
        flashAlpha += 0.005f;
        if (flashAlpha > 1) {
            flashAlpha = 0.0f;
        }
        break;
    case MODIF_OMNI:
        rotateAngle += 0.1;
        scaleAngle += 0.025;
        if (scaleAngle >= 360)
            scaleAngle = 0.0;
        xScale = (cos(scaleAngle) / 4.0) + 0.75;
        yScale = (sin(scaleAngle) / 4.0) + 0.75;
        windowPosX -= (ballX - xBefore) * xScale;
        windowPosY += (ballY - yBefore) * yScale;
        glutPositionWindow(windowPosX, windowPosY);
        same = true;
        for (int i = 0; i < 2; i++) {
            if (goalRGB[i] != rgb[i]) {
                same = false;
            }
        }
        if (same) {
            int randVal;
            for (int i = 0; i < 3; i++) {
                randVal = (rand() % 50) + 50;
                goalRGB[i] = randVal;
            }
        }
        else {
            int randVal = rand() % 3;
            if (rgb[randVal] > goalRGB[randVal]) {
                rgb[randVal] -= 1;
            }
            else if (rgb[randVal] < goalRGB[randVal]) {
                rgb[randVal] += 1;
            }
        }
        vec.push_back(ballX);
        vec.push_back(ballY);
        previousBallPositions.insert(previousBallPositions.begin(), vec);
        break;
    }
}

void switchModifier(bool ran) {
    int oldModif = modifier;
    float randVal;
    float sumProbs = 0;
    float probs[numModifiers];
    if (ran) {
        for (int i = 0; i < numModifiers; i++) {
            sumProbs += modifProbs[i]; // Calculate probability sum
        }
        for (int i = 0; i < numModifiers; i++) {
            probs[i] = modifProbs[i] / sumProbs; // Normalize probabiltiies
        }
        for (int i = 1; i < numModifiers; i++) {
            probs[i] = probs[i] + probs[i - 1]; // Determine cummulative probabilities
        }
        randVal = randomFloat();
        modifier = 0;
        for (int i = 1; i < numModifiers; i++) {
            if (randVal > probs[i - 1] && randVal <= probs[i]) { modifier = i; }
        }
        //modifier = rand() % numModifiers;
        if (modifier == oldModif) {
            modifier++;
            modifier %= numModifiers;
        }
    }
    else {
        modifier++;
        if (isAI == 2 && modifier == 3)
            modifier++;
        modifier = modifier % numModifiers;
    }

    switch (modifier) {
    case MODIF_ROTATE:
        rotateAngle = 0.0;
        break;
    case MODIF_TILT:
        randAngle = rand() % 361;
        break;
    case MODIF_SUPER:
        if (isAI == 2) {
            modifier = MODIF_NONE;
        }
        break;
    case MODIF_SCALE:
        randVal = (rand() % 50) + 51;
        xScale = (randVal / 100.0);
        randVal = (rand() % 50) + 51;
        yScale = (randVal / 100.0);
        break;
    case MODIF_WOOZY:
        xScale = 1.0;
        yScale = 1.0;
        break;
    case MODIF_DIZZY:
        rotateAngle = 0.0;
        xScale = 1.0;
        yScale = 1.0;
        break;
    case MODIF_STABLE:
        windowPosX = glutGet((GLenum)GLUT_WINDOW_X);
        windowPosY = glutGet((GLenum)GLUT_WINDOW_Y);
        break;
    case MODIF_GAMER:
        for (int i = 0; i < 3; i++) {
            goalRGB[i] = (rand() % 50) + 50;
        }
        break;
    case MODIF_SNONG:
        previousBallPositions.clear();
        break;
    case MODIF_FLASH:
        flashAlpha = 0.0f;
        break;
    case MODIF_OMNI:
        rotateAngle = 0.0;
        xScale = 1.0;
        yScale = 1.0;
        windowPosX = glutGet((GLenum)GLUT_WINDOW_X);
        windowPosY = glutGet((GLenum)GLUT_WINDOW_Y);
        for (int i = 0; i < 3; i++) {
            goalRGB[i] = (rand() % 50) + 50;
        }
        previousBallPositions.clear();
        break;
    case MODIF_PONGLE:
        struct Point pongleSpaceOffsetFromOrigin = windowSize * 0.1 + pegSize;
        struct Point pongleSpaceSize = windowSize - pongleSpaceOffsetFromOrigin * 2;
        std::cout << "Creating pegs in space " << pongleSpaceSize << " offset by " << pongleSpaceOffsetFromOrigin << std::endl;

        for (int i = 0; i < pegCount; i++) {
            pegs[i].center = randomIn(pongleSpaceSize) + pongleSpaceOffsetFromOrigin;
            //std::cout << pegs[i].center << std::endl;
            pegs[i].radius = pegSize;
        }
        break;
    }
    cout << modifier << endl;
}

void idle() {
    thisFrameTime = now();
    if (lastFrameTime == -1) lastFrameTime = thisFrameTime;
    deltaTime = (thisFrameTime - lastFrameTime) / 1000.0;

    updateDrag();
    updatePaddles();

    if (modifier != MODIF_SUPER || (modifier == MODIF_SUPER && super)) {
        updateBall();
        super = false;
    }

    updateAI();
    updateModifier();

    lastFrameTime = thisFrameTime;

    glutPostRedisplay();
}

void timer(int _) {
    if (game_mode == MODE_VS_AI || game_mode == MODE_VS_PLAYER || game_mode == MODE_AI_VS_AI) {
        idle();
    }

    if (game_mode == MODE_WIN_PAUSE) {
        cooldown--;

        if (cooldown == 0) {
            reset();
            switchModifier(true);
            switch (isAI) {
            case 0:
                setGameMode(MODE_VS_PLAYER);
                break;
            case 1:
                setGameMode(MODE_VS_AI);
                break;
            case 2:
                setGameMode(MODE_AI_VS_AI);
                break;
            };

            //activeBallTexture = (activeBallTexture + 1) % size(ball_textures);
            //activePaddleTexture = (activePaddleTexture + 1) % size(paddle_textures);
        }

        glutPostRedisplay();
    }

    glutTimerFunc(millisecondsPerFrame, timer, _);
}

void keyboard(unsigned char key, int x, int y) {
    keyboardDown[key] = true;
    switch (key) {

    case 'r':
        reset();
        switchModifier(true);
        switch (isAI) {
        case 0:
            setGameMode(MODE_VS_PLAYER);
            break;
        case 1:
            setGameMode(MODE_VS_AI);
            break;
        case 2:
            setGameMode(MODE_AI_VS_AI);
            break;
        };

        //activeBallTexture = (activeBallTexture + 1) % size(ball_textures);
        //activePaddleTexture = (activePaddleTexture + 1) % size(paddle_textures);

        break;

    case 'p':
        if (game_mode == MODE_VS_AI || game_mode == MODE_VS_PLAYER || game_mode == MODE_AI_VS_AI) {
            setGameMode(MODE_PAUSE);
        }
        else if (game_mode == MODE_PAUSE) {
            switch (isAI) {
            case 0:
                setGameMode(MODE_VS_PLAYER);
                break;
            case 1:
                setGameMode(MODE_VS_AI);
                break;
            case 2:
                setGameMode(MODE_AI_VS_AI);
                break;
            };
            lastFrameTime = now();
            deltaTime = 0;
        }
        break;

    case 'm':
        switchModifier(false);
        break;

    case 27:
        //exit(0);
        modifier = MODIF_NONE;
        setGameMode(MODE_TITLE);
        break;
    }
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) { keyboardDown[key] = false; }

void special(int key, int x, int y) { specialDown[key] = true; }

void specialUp(int key, int x, int y) { specialDown[key] = false; }

void init() {

    //cout << windowWidth << " | " << windowHeight << endl;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    glPointSize(4);

    // Enable alpha transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


    srand(time(0));
    reset();

}

void onBallTextureMenu(int option) {
    activeBallTexture = option;
    glutPostRedisplay();
}

void onPaddleOneTextureMenu(int option) {
    activePaddleOneTexture = option;
    glutPostRedisplay();
}

void onPaddleTwoTextureMenu(int option) {
    activePaddleTwoTexture = option;
    glutPostRedisplay();
}

void onPaddleOneColorMenu(int option) {
    activePaddleOneColor = option;
    glutPostRedisplay();
}

void onPaddleTwoColorMenu(int option) {
    activePaddleTwoColor = option;
    glutPostRedisplay();
}

void onContextMenu(int option) { }

void initTextureMenu(void) {
    GLuint ballTextureMenuID = glutCreateMenu(onBallTextureMenu);
    const char* ballLabels[] = {
        "None", "Ping-Pong", "Baseball", "Basketball", "Orange",
    };

    for (int i = 0; i < sizeof(ballLabels) / sizeof(char*); i++) {
        glutAddMenuEntry(ballLabels[i], i - 1);
    }

    GLuint paddleOneTextureMenuID = glutCreateMenu(onPaddleOneTextureMenu);
    const char* paddleOneLabels[] = {
        "None", "Barber Pole", "French Fry", "Paddles", "Swords"
    };

    for (int i = 0; i < sizeof(paddleOneLabels) / sizeof(char*); i++) {
        glutAddMenuEntry(paddleOneLabels[i], i - 1);
    }

    GLuint paddleTwoTextureMenuID = glutCreateMenu(onPaddleTwoTextureMenu);
    const char* paddleTwoLabels[] = {
        "None", "Barber Pole", "French Fry", "Paddles", "Swords"
    };

    for (int i = 0; i < sizeof(paddleTwoLabels) / sizeof(char*); i++) {
        glutAddMenuEntry(paddleTwoLabels[i], i - 1);
    }

    GLuint paddleOneColorMenuID = glutCreateMenu(onPaddleOneColorMenu);
    const char* paddleOneColorLabels[] = {
        "White", "Red", "Orange", "Yellow", "Green", "Blue", "Purple", "Cyan", "Gray", "Brown"
    };

    for (int i = 0; i < sizeof(paddleOneColorLabels) / sizeof(char*); i++) {
        glutAddMenuEntry(paddleOneColorLabels[i], i - 1);
    }

    GLuint paddleTwoColorMenuID = glutCreateMenu(onPaddleTwoColorMenu);
    const char* paddleTwoColorLabels[] = {
        "White", "Red", "Orange", "Yellow", "Green", "Blue", "Purple", "Cyan", "Gray", "Brown"
    };

    for (int i = 0; i < sizeof(paddleTwoColorLabels) / sizeof(char*); i++) {
        glutAddMenuEntry(paddleTwoColorLabels[i], i - 1);
    }

    GLuint contextMenuID = glutCreateMenu(onContextMenu);
    glutAddSubMenu("Ball texture", ballTextureMenuID);
    glutAddSubMenu("Paddle one texture", paddleOneTextureMenuID);
    glutAddSubMenu("Paddle two texture", paddleTwoTextureMenuID);
    glutAddSubMenu("Paddle one color", paddleOneColorMenuID);
    glutAddSubMenu("Paddle two color", paddleTwoColorMenuID);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

#include "Test.cpp"

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Pong with Multiversal Instability");
    glewInit();

    titleInit();
    glutDisplayFunc(titleDisplay);
    glutKeyboardFunc(titleKeyboard);
    glutIdleFunc(titleIdle);
    modifOptionInit();

    glutSpecialUpFunc(specialUp);
    glutKeyboardUpFunc(keyboardUp);

    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    setGameMode(MODE_TITLE);

    title.init();
    title.xywh(0, windowHeight - 200, 1000, 200);

    // Not needed anymore unless we want a settings option to not have the spinning screen
    background_title.init();
    background_title.xywh(0, 0, windowWidth, windowHeight);

    background_swirl.init();
    background_swirl.xywh(-100, -283, 1200, 1200);

    board.init();
    board.xywh(0, 0, windowWidth, windowHeight - 31);

    for (TexturedRectangle* r : ball_textures) {
        r->init();
        r->xywh(0, 0, ballDiameter, ballDiameter);
    }

    for (TexturedRectangle* r : paddle_textures) {
        r->init();
        r->xywh(0, 0, 10, 100);
    }
    initTextureMenu();
    test();

    SoundEngine->play2D("audio/bgm.wav", true);
    SoundEngine->setSoundVolume(volume); // "volume" is in MenuScreens.cpp;

    glutMainLoop();
    return 0;
}