#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>
//#include "MenuScreens.cpp" // Originally a separate file for storing the title screen functions. Deprecated because of complexity (for now)

#define PI 3.14159265
#define TARGET_FPS 30

using namespace std;

const int windowHeight = 630;
const int windowWidth = 1000;
const int millisecondsPerFrame = int(1000.0 / TARGET_FPS);

// Paddle 1
float p1x1, p1y1, p1x2, p1y2;

// Paddle 2
float p2x1, p2y1, p2x2, p2y2;
bool isAI = false;
float p2y;

const float paddleSpeed = 10.0;

// Ball
float ballX, ballY, ballSpeedX, ballSpeedY;
const float ballSpeed = 8;
int speedUp = 0;

// Scoring
int player1Score = 0;
int player2Score = 0;

bool isPaused = false;

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

// Centralize state management in setGameMode function
// At the moment, this only controls setting state for the timer function, since that's what Mark is working on
#define MODE_TITLE 2
#define MODE_VS_PLAYER 0
#define MODE_VS_AI 1
#define MODE_WIN_PAUSE 3
#define MODE_PAUSE 4
static int game_mode = MODE_TITLE;

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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 5, ((windowHeight - 30) / 2.0) - 1, (windowWidth / 2.0) + 5, ((windowHeight - 30) / 2.0) + 1);
    glRectf((windowWidth / 2.0) - 1, 1, (windowWidth / 2.0) + 1, windowHeight - 31);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES); // Draw Play Area

    glVertex2i(1, 1);
    glVertex2i(windowWidth - 1, 1);

    glVertex2i(windowWidth - 1, 1);
    glVertex2i(windowWidth - 1, windowHeight - 31);

    glVertex2i(windowWidth - 1, windowHeight - 31);
    glVertex2i(1, windowHeight - 31);

    glVertex2i(1, windowHeight - 31);
    glVertex2i(1, 1);

    glEnd();

    // Paddle 1

    glRectf(p1x1, p1y1, p1x2, p1y2);

    // Paddle 2

    glRectf(p2x1, p2y1, p2x2, p2y2);

    // Ball
    glRectf(ballX - 4, ballY - 4, ballX + 4, ballY + 4);

    // Render Score
    string score = to_string(player1Score) + " | " + to_string(player2Score);
    int scoreLength = to_string(player1Score).length();
    renderText(score, 491.0 - scoreLength * 12.0, 606.0);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glutReshapeWindow(windowWidth, windowHeight);
}

void reset() {

    //cout << windowWidth << " | " << windowHeight << endl;

    p1x1 = 10.0;
    p1y1 = ((windowHeight - 30) / 2.0) + 50.0;
    p1x2 = 20.0;
    p1y2 = ((windowHeight - 30) / 2.0) - 50.0;

    p2x1 = windowWidth - 10.0;
    p2y1 = ((windowHeight - 30) / 2.0) + 50.0;
    p2x2 = windowWidth - 20.0;
    p2y2 = ((windowHeight - 30) / 2.0) - 50.0;
    p2y = ((windowHeight - 30) / 2.0);

    ballX = windowWidth / 2.0;
    ballY = (windowHeight - 30) / 2.0;
    speedUp = 0;

    int rdm = rand() % 4 + 1;
    if (rdm == 1) {
        rdm = (rand() % 60) + 15.0;
    }
    else if (rdm == 2) {
        rdm = (rand() % 60) + 105.0;
    }
    else if (rdm == 3) {
        rdm = (rand() % 60) + 195.0;
    }
    else {
        rdm = (rand() % 60) + 285.0;
    }
    //cout << rdm << endl;
    ballSpeedX = ballSpeed * cos((rdm * PI / 180.0));
    ballSpeedY = ballSpeed * sin((rdm * PI / 180.0));
    //cout << "X: " << ballSpeedX << " | Y: " << ballSpeedY << endl;
}


void timer(int _) {
    if (game_mode == MODE_VS_AI || game_mode == MODE_VS_PLAYER) {
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
        default:
            std::cerr << "Warning: setGameMode was called with invalid mode " << mode << std::endl;
            modeIsValid = false;
            break;
    }
    if (modeIsValid)
        game_mode = mode;
}

void idle() {

    if ((ballY + 5 >= windowHeight - 31) || (ballY - 5 <= 1)) {
        ballSpeedY = 0.0 - ballSpeedY;
    }
    else if (ballX <= p1x2 && ballX >= p1x1 && ballY >= p1y2 && ballY <= p1y1) {
        ballX = p1x2;
        ballSpeedX = 0 - ballSpeedX;
        speedUp += 1;
        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }
    }
    else if (ballX >= p2x2 && ballX <= p2x1 && ballY >= p2y2 && ballY <= p2y1) {
        ballX = p2x2;
        ballSpeedX = 0 - ballSpeedX;
        speedUp += 1;
        if (speedUp % 3 == 0) {
            ballSpeedX *= 1.05;
            ballSpeedY *= 1.05;
        }
    }
    else if ((ballX + 5 >= windowWidth - 1)) {
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
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    int rdm = (rand() % 2); // (rand() % 175); && rdm >= 175
    if (isAI && rdm >= 1 && ballX > (windowWidth / 2.0) && ballSpeedX > 0) { // AI
        if (ballY >= (p2y + 30) && p2y1 <= windowHeight - 40) {
            p2y1 += paddleSpeed;
            p2y2 += paddleSpeed;
            p2y += paddleSpeed;
        }
        else if (ballY <= (p2y - 30) && p2y2 >= 10) {
            p2y1 -= paddleSpeed;
            p2y2 -= paddleSpeed;
            p2y -= paddleSpeed;
        }
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    
    case 'w':
        if (p1y1 <= windowHeight - 40) {
            p1y1 += paddleSpeed;
            p1y2 += paddleSpeed;
        }
        break;
    case 's':
        if (p1y2 >= 10) {
            p1y1 -= paddleSpeed;
            p1y2 -= paddleSpeed;
        }
        break;

    case 'r':
        reset();
        setGameMode(isAI ? MODE_VS_AI : MODE_VS_PLAYER);
        break;

    case 'p':
        if (isPaused) {
            isPaused = false;
            setGameMode(isAI ? MODE_VS_AI : MODE_VS_PLAYER);
        }
        else {
            isPaused = true;
            setGameMode(MODE_PAUSE);
        }
        break;

    case 27:
        //exit(0);
        setGameMode(MODE_TITLE);
        break;
    }
    glutPostRedisplay();
}

void special(int key, int x, int y) {

    switch (key) {
    case GLUT_KEY_UP:
        if (p2y1 <= windowHeight - 40) {
            p2y1 += paddleSpeed;
            p2y2 += paddleSpeed;
        }
        break;
    case GLUT_KEY_DOWN:
        if (p2y2 >= 10) {
            p2y1 -= paddleSpeed;
            p2y2 -= paddleSpeed;
        }
        break;
    }
    glutPostRedisplay();
}

void init() {

    //cout << windowWidth << " | " << windowHeight << endl;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

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
            isAI = false;
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
            isAI = true;
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

    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    setGameMode(MODE_TITLE);

    glutMainLoop();
    return 0;
}