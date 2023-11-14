#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <chrono>
#include <random>
//#include "MenuScreens.cpp" // Originally a separate file for storing the title screen functions. Deprecated because of complexity (for now)

#include "Paddle.cpp"

#define PI 3.14159265
#define TARGET_FPS 60.0
static int frameCount = 0;
static long lastSecond = -1;
static long lastFrameTime = -1; // In milliseconds
static long thisFrameTime; // In milliseconds
static float deltaTime; // In seconds

using namespace std;

const int windowHeight = 630;
const int windowWidth = 1000;
const int millisecondsPerFrame = int(1000.0 / TARGET_FPS);

struct Paddle p1, p2;

int isAI = 2;

const float paddleSpeed = 300; // in pixels per second

// Ball
float ballX, ballY, ballSpeedX, ballSpeedY;
const float ballSpeed = 240; // in pixels per second
int speedUp = 0;

// Scoring
int player1Score = 0;
int player2Score = 0;

// Title Screen
string title;
int titleLen;
string PVP, PVE;

// Function Initialization
void titleInit();
void titleDisplay();
void idle();
void titleMouse(int button, int state, int x, int y);
void titleKeyboard(unsigned char key, int x, int y);

// Modifiers
int randAngle = rand() % 361;
float rotateAngle = 0;

bool super = false;

// Centralize state management in setGameMode function
// At the moment, this only controls setting state for the timer function, since that's what Mark is working on
#define MODE_TITLE 2
#define MODE_VS_PLAYER 0
#define MODE_VS_AI 1
#define MODE_WIN_PAUSE 3
#define MODE_PAUSE 4
#define MODE_AI_VS_AI 5
static int game_mode = MODE_TITLE;

// Modifier variable
#define MODIF_NONE 0
#define MODIF_ROTATE 1
#define MODIF_TILT 2
#define MODIF_SUPER 3
static int modifier = MODIF_NONE;
const int numModifiers = 4;

// Track what keys are down for smooth updates.
bool keyboardDown[255] = {}; // To check for 'a' key, do keyboardDown['a']. Single quote characters are ints in C++
bool specialDown[255] = {}; // To check for left key, do specialDown[GLUT_KEY_LEFT]

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

long now() {
    return std::chrono::duration_cast< std::chrono::milliseconds >(
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
    else if (modifier == MODIF_ROTATE) {
        glTranslatef((windowWidth / 2), (windowHeight / 2), 0);
        glRotatef(rotateAngle, 0.0, 0.0, 1.0);
        glTranslatef(-(windowWidth / 2), -(windowHeight / 2), 0);
        glScalef(0.5, 0.5, 1);
        glTranslatef(windowWidth / 2, windowHeight / 2, 0);
    }

    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 5, ((windowHeight - 30) / 2.0) - 1, (windowWidth / 2.0) + 5, ((windowHeight - 30) / 2.0) + 1);
    glRectf((windowWidth / 2.0) - 1, 1, (windowWidth / 2.0) + 1, windowHeight - 31);
    glColor3f(1.0f, 1.0f, 1.0f);

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
    paddleDraw(p1);
    paddleDraw(p2);

    // Paddle Paths
    // bezierDraw(p1.path);
    // bezierDraw(p2.path);

    // Ball
    glRectf(ballX - 4, ballY - 4, ballX + 4, ballY + 4);

    // Render Score
    string score = to_string(player1Score) + " | " + to_string(player2Score);
    int scoreLength = to_string(player1Score).length();
    renderText(score, 491.0 - scoreLength * 12.0, 606.0);

    frameCount++;
    if (now() - lastSecond >= 1000) {
        printf("FPS: %d  \r", frameCount);
        std::cout.flush();
        frameCount = 0;
        lastSecond = now();
    }

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glutReshapeWindow(windowWidth, windowHeight);
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

    float startAngles[] = {15.0, 105.0, 195.0, 285.0};
    float degrees = startAngles[rand() % 4] + rand() % 60;
    float radians = degrees * PI / 180.0;

    ballSpeedX = ballSpeed * cos(radians);
    ballSpeedY = ballSpeed * sin(radians);

    lastFrameTime = -1;
    lastSecond = -1;
}


void timer(int _) {
    if (game_mode == MODE_VS_AI || game_mode == MODE_VS_PLAYER || game_mode == MODE_AI_VS_AI) {
        idle();
    }

    glutTimerFunc(millisecondsPerFrame, timer, _);
}

void setGameMode(int mode) {
    bool modeIsValid = true;
    switch (mode) {
        case MODE_VS_AI:
            break;
        case MODE_TITLE:
            // Moved the functionality for resetting to the title screen here (Zach)
            player1Score = 0;
            player2Score = 0;
            titleInit();
            glutDisplayFunc(titleDisplay);
            glutKeyboardFunc(titleKeyboard);
            glutMouseFunc(titleMouse);
            glutSpecialFunc(NULL);
            break;
        case MODE_VS_PLAYER:
            break;
        case MODE_PAUSE:
            break;
        case MODE_WIN_PAUSE:
            break;
        case MODE_AI_VS_AI:
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

void updatePaddles() {

    if (game_mode == MODE_VS_PLAYER || game_mode == MODE_VS_AI) {
        if (keyboardDown['w']) {
            if (p1.y1 <= windowHeight - 40) {
                paddleMoveT(p1, p2.tOffset);

                if (modifier == MODIF_SUPER) super = true;
            }
        }
        if (keyboardDown['s']) {
            if (p1.y2 >= 1) {
                paddleMoveT(p1, -p2.tOffset);

                if (modifier == MODIF_SUPER) super = true;
            }
        }
    }
    
    if (game_mode == MODE_VS_PLAYER) {
        if (specialDown[GLUT_KEY_UP]) {
            if (p2.y1 <= windowHeight - 40) {
                paddleMoveT(p2, p2.tOffset);

                if (modifier == MODIF_SUPER) super = true;
            }
        }
        if (specialDown[GLUT_KEY_DOWN]) {
            if (p2.y2 >= 1) {
                paddleMoveT(p2, -p2.tOffset);

                if (modifier == MODIF_SUPER) super = true;
            }
        }
    }
}

void updateBall() {
    if (ballY + 5 >= windowHeight - 31) {
        ballSpeedY = -abs(ballSpeedY);
    } else if (ballY - 5 <= 0) {
        ballSpeedY = abs(ballSpeedY);
    }

    if (paddleContains(p1, ballX, ballY)) {
        ballX = p1.x2;
        ballSpeedX = 0 - ballSpeedX;
        speedUp += 1;
        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }
    }
    else if (paddleContains(p2, ballX, ballY)) {
        ballX = p2.x1;
        ballSpeedX = 0 - ballSpeedX;
        speedUp += 1;
        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }
    }

    if ((ballX + 5 >= windowWidth)) {
        //cout << "Player 1 wins the game" << endl;
        player1Score += 1;
        //reset();
        setGameMode(MODE_WIN_PAUSE);
        glutPostRedisplay();
        return;
    }
    else if ((ballX - 5 <= 1)) {
        //cout << "Player 2 wins the game" << endl;
        player2Score += 1;
        //reset();
        setGameMode(MODE_WIN_PAUSE);
        glutPostRedisplay();
        return;
    }
    
    ballX += ballSpeedX * deltaTime;
    ballY += ballSpeedY * deltaTime;
}

void updateAI() {

    if (isAI == 1) {
        int rdm = (rand() % 2); // (rand() % 175); && rdm >= 175
        if (isAI && rdm >= 1 && ballX > (windowWidth / 2.0) && ballSpeedX > 0) { // AI
            if (ballY >= (p2.y1 - 20) && p2.y1 <= windowHeight - 40) {
                paddleMoveT(p2, p2.tOffset);
            }
            else if (ballY <= (p2.y2 + 20) && p2.y2 >= 10) {
                paddleMoveT(p2, -p2.tOffset);
            }
        }
    }
    else if (isAI == 2) {

        if (ballX > (windowWidth / 2.0) && ballSpeedX > 0) { // AI
            if (ballY >= (p2.y1 - 20) && p2.y1 <= windowHeight - 40) {
                paddleMoveT(p2, p2.tOffset);
            }
            else if (ballY <= (p2.y2 + 20) && p2.y2 >= 10) {
                paddleMoveT(p2, -p2.tOffset);
            }
        }

        if (ballSpeedX > 0) {
            if (ballY >= (p1.y1 - 20) && p1.y1 <= windowHeight - 40) {
                paddleMoveT(p1, p1.tOffset);
            }
            else if (ballY <= (p1.y2 + 20) && p1.y2 >= 10) {
                paddleMoveT(p1, -p1.tOffset);
            }
        }

    }
}

void updateModifier() {
    switch (modifier) {
    case MODIF_ROTATE:
        rotateAngle += 0.1;
        break;
    }
}

void switchModifier(bool ran) {
    
    if (ran) {
        modifier = rand() % numModifiers;
    }
    else {
        modifier = (modifier + 1) % numModifiers;
    }
    
    switch (modifier) {
    case MODIF_ROTATE:
        rotateAngle = 0.0;
        break;
    case MODIF_TILT:
        randAngle = rand() % 361;
        break;
    }
}

void idle() {
    thisFrameTime = now();
    if (lastFrameTime == -1) lastFrameTime = thisFrameTime;
    deltaTime = (thisFrameTime - lastFrameTime) / 1000.0;

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

        break;

    case 'p':
        if (game_mode == MODE_VS_AI || game_mode == MODE_VS_PLAYER || game_mode == MODE_AI_VS_AI) {
            setGameMode(MODE_PAUSE);
        } else if (game_mode == MODE_PAUSE) {
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
        }
        break;

    case 'm':
        switchModifier(false);
        break;

    case 27:
        //exit(0);
        setGameMode(MODE_TITLE);
        break;
    }
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) { keyboardDown[key] = false; }


void special(int key, int x, int y)   { specialDown[key] = true; }

void specialUp(int key, int x, int y) { specialDown[key] = false; }

void init() {

    //cout << windowWidth << " | " << windowHeight << endl;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    glPointSize(4);

    srand(time(0));
    reset();

}

// -- Title Screen Functions --

void titleInit() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    title = "PONG WITH MULTIVERSAL INSTABILITY";
    titleLen = title.length() / 2.0;

    PVP = "Player vs. Player";
    PVE = "Player vs. AI";
}

void titleDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Central Guide Line
    //    glColor3f(0.5f, 0.5f, 0.5f);
    //    glRectf((menuWindowWidth / 2.0) - 1, 0, (menuWindowWidth / 2.0) + 1, menuWindowHeight);
    //    glColor3f(1.0f, 1.0f, 1.0f);

    // Title
    renderText(title, (windowWidth / 2.0) - (titleLen * 15.0), 500.0);

    // Player Vs. Player
    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 85, 375, (windowWidth / 2.0) + 85, 425);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(PVP, (windowWidth / 2.0) - 80, 390.0);


    // Player Vs. AI
    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 70, 275, (windowWidth / 2.0) + 70, 325);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(PVE, (windowWidth / 2.0) - 63, 290.0);

    glutSwapBuffers();
}

void titleKeyboard(unsigned char key, int x, int y) {
    keyboardDown[key] = true;
    switch (key) {
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void titleMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = windowHeight - y;
        //cout << "Mouse Click | X: " << x << "| Y: " << y << endl;

        if ((x >= ((windowWidth / 2.0) - 80)) && (x <= ((windowWidth / 2.0) + 80)) && (y >= 375) && (y <= 425)) {
            cout << PVP << endl;
            init();
            glutMouseFunc(NULL);
            glutKeyboardFunc(keyboard);
            //glutKeyboardUpFunc();
            glutSpecialFunc(special);
            glutDisplayFunc(display);
            isAI = 0;
            setGameMode(MODE_VS_PLAYER);
            glutPostRedisplay();
        }
        else if ((x >= ((windowWidth / 2.0) - 70)) && (x <= ((windowWidth / 2.0) + 70)) && (y >= 275) && (y <= 325)) {
            cout << PVE << endl;
            init();
            glutMouseFunc(NULL);
            glutKeyboardFunc(keyboard);
            //glutKeyboardUpFunc();
            //glutSpecialFunc(special); // Not needed because AI has control of paddle 2.
            glutDisplayFunc(display);
            isAI = 1;
            setGameMode(MODE_VS_AI);
            glutPostRedisplay();
        }
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Pong with Multiversal Instability");

    titleInit();
    glutDisplayFunc(titleDisplay);
    glutKeyboardFunc(titleKeyboard);
    glutMouseFunc(titleMouse);
    
    glutSpecialUpFunc(specialUp);
    glutKeyboardUpFunc(keyboardUp);

    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    setGameMode(MODE_TITLE);

    glutMainLoop();
    return 0;
}