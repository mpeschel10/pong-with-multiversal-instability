#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string>

#define PI 3.14159265

using namespace std;

int windowHeight;
int windowWidth;

// Paddle 1
float p1x1;
float p1y1;
float p1x2;
float p1y2;

// Paddle 2
float p2x1;
float p2y1;
float p2x2;
float p2y2;

const float paddleSpeed = 10.0;

// Ball
float ballX;
float ballY;
float ballSpeedX;
float ballSpeedY;
float ballSpeed = 0.08;

// Scoring
int player1Score = 0;
int player2Score = 0;

bool isPaused = false;

void renderText(const string& text, float x, float y) {
    // Professor Reza's Render Text Function
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 650); // Modified for the window size of our application

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
    glRectf((windowWidth / 2.0) - 5, ((windowHeight - 50) / 2.0) - 1, (windowWidth / 2.0) + 5, ((windowHeight - 50) / 2.0) + 1);
    glRectf((windowWidth / 2.0) - 1, 0, (windowWidth / 2.0) + 1, windowHeight - 51);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_LINES); // Draw Play Area
    
    glVertex2i(1, 1);
    glVertex2i(windowWidth - 1, 1);

    glVertex2i(windowWidth - 1, 1);
    glVertex2i(windowWidth - 1, windowHeight - 1);

    glVertex2i(windowWidth - 1, windowHeight -1);
    glVertex2i(1, windowHeight -1);

    glVertex2i(1, windowHeight - 1);
    glVertex2i(1, 1);

    glEnd();

    // Paddle 1

    glRectf(p1x1, p1y1, p1x2, p1y2);

    // Paddle 2

    glRectf(p2x1, p2y1, p2x2, p2y2);

    // Ball
    glRectf(ballX - 5, ballY - 5, ballX + 5, ballY + 5);

    // Render Score
    string score = to_string(player1Score) + " | " + to_string(player2Score);
    int scoreLength = to_string(player1Score).length();
    //renderText(score, (windowWidth / 2), (windowHeight - 50));
    renderText(score, 491.0 - scoreLength*12.0, 623.0);
    
    glFlush();
}

void reshape(int width, int height) {
    glutReshapeWindow(1000, 650);
}

void reset() {
    windowWidth = glutGet(GLUT_SCREEN_WIDTH);
    windowHeight = glutGet(GLUT_SCREEN_HEIGHT);

    p1x1 = 10.0;
    p1y1 = ((windowHeight - 50) / 2.0) + 50.0;
    p1x2 = 20.0;
    p1y2 = ((windowHeight - 50) / 2.0) - 50.0;

    p2x1 = windowWidth - 10.0;
    p2y1 = ((windowHeight - 50) / 2.0) + 50.0;
    p2x2 = windowWidth - 20.0;
    p2y2 = ((windowHeight - 50) / 2.0) - 50.0;

    ballX = windowWidth / 2.0;
    ballY = (windowHeight - 50) / 2.0;

    /*int rdm = rand() % 4 + 1;
    if (rdm == 1) {
    ballSpeedX = -ballSpeed;
    ballSpeedY = ballSpeed;
    }
    else if (rdm == 2) {
        ballSpeedX = -ballSpeed;
        ballSpeedY = -ballSpeed;
    }
    else if (rdm == 3) {
        ballSpeedX = ballSpeed;
        ballSpeedY = ballSpeed;
    }
    else {
        ballSpeedX = ballSpeed;
        ballSpeedY = -ballSpeed;
    } */

    int rdm = rand() % 4 + 1;
    if (rdm == 1) {
        rdm = (rand() % 60) + 15;
    }
    else if (rdm == 2) {
        rdm = (rand() % 60) + 105;
    }
    else if (rdm == 3) {
        rdm = (rand() % 60) + 195;
    }
    else {
        rdm = (rand() % 60) + 285;
    }
    ballSpeedX = ballSpeed * cos((rdm * PI / 180.0));
    ballSpeedY = ballSpeed * sin((rdm * PI / 180.0));
}

void idle() {
    if ((ballY + 5 >= windowHeight - 51) || (ballY - 5 <= 1)) {
        ballSpeedY = 0.0 - ballSpeedY;
    }
    else if (ballX <= p1x2 && ballX >= p1x1 && ballY >= p1y2 && ballY <= p1y1) {
        ballX = p1x2;
        ballSpeedX = 0.0 - ballSpeedX;
    }
    else if (ballX >= p2x2 && ballX <= p2x1 && ballY >= p2y2 && ballY <= p2y1) {
        ballX = p2x2;
        ballSpeedX = 0.0 - ballSpeedX;
    }
    else if ((ballX + 5 >= windowWidth - 1)) {
        cout << "Player 1 wins the game" << endl;
        //reset();
        glutIdleFunc(NULL);
        glutPostRedisplay();
        return;
    }
    else if ((ballX - 5 <= 1)) {
        cout << "Player 2 wins the game" << endl;
        //reset();
        glutIdleFunc(NULL);
        glutPostRedisplay();
        return;
    }
    ballX += ballSpeedX;
    ballY += ballSpeedY;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    
    case 'w': 
        if (p1y1 <= windowHeight - 60) {
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
        glutIdleFunc(idle);
        break;

    case 'p':
        if (isPaused) {
            isPaused = false;
            glutIdleFunc(idle);
        }
        else {
            isPaused = true;
            glutIdleFunc(NULL);
        }
        break;
    
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void special(int key, int x, int y) {
   
    switch (key) {
    case GLUT_KEY_UP:
        if (p2y1 <= windowHeight - 60) {
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
    
    windowWidth = glutGet(GLUT_SCREEN_WIDTH);
    windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    srand(time(0));
    reset();
    
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 650);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Pong with Multiversal Instability");

    init();

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
