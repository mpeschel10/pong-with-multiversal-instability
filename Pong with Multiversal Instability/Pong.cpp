#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
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

// Ball
float ballX;
float ballY;
float ballSpeedX;
float ballSpeedY;

const float paddleSpeed = 10.0;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

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

    glFlush();
}

void reshape(int width, int height) {
    glutReshapeWindow(1000, 600);
}

void reset() {
    windowWidth = glutGet(GLUT_SCREEN_WIDTH);
    windowHeight = glutGet(GLUT_SCREEN_HEIGHT);

    p1x1 = 10.0;
    p1y1 = (windowHeight / 2.0) + 50.0;
    p1x2 = 20.0;
    p1y2 = (windowHeight / 2.0) - 50.0;

    p2x1 = windowWidth - 10.0;
    p2y1 = (windowHeight / 2.0) + 50.0;
    p2x2 = windowWidth - 20.0;
    p2y2 = (windowHeight / 2.0) - 50.0;

    ballX = windowWidth / 2.0;
    ballY = windowHeight / 2.0;

    int rdm = rand() % 4 + 1;
    if (rdm == 1) {
        ballSpeedX = -0.08;
        ballSpeedY = 0.08;
    }
    else if (rdm == 2) {
        ballSpeedX = -0.08;
        ballSpeedY = -0.08;
    }
    else if (rdm == 3) {
        ballSpeedX = 0.08;
        ballSpeedY = 0.08;
    }
    else {
        ballSpeedX = 0.08;
        ballSpeedY = -0.08;
    }
}

void idle() {
    if ((ballY + 5 >= windowHeight - 1) || (ballY - 5 <= 1)) {
        ballSpeedY = 0.0 - ballSpeedY;
    }
    else if (ballX <= p1x2 && ballX >= p1x1 && ballY >= p1y2 && ballY <= p1y1) {
        ballSpeedX = 0.0 - ballSpeedX;
    }
    else if (ballX >= p2x2 && ballX <= p2x1 && ballY >= p2y2 && ballY <= p2y1) {
        ballSpeedX = 0.0 - ballSpeedX;
    }
    else if ((ballX + 5 >= windowWidth - 1)) {
        cout << "Player 1 wins the game" << endl;
        //reset();
        glutIdleFunc(NULL);
        return;
    }
    else if ((ballX - 5 <= 1)) {
        cout << "Player 2 wins the game" << endl;
        //reset();
        glutIdleFunc(NULL);
        return;
    }
    ballX += ballSpeedX;
    ballY += ballSpeedY;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    
    case 'w': 
        if (p1y1 <= windowHeight - 10) {
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
    
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void special(int key, int x, int y) {
   
    switch (key) {
    case GLUT_KEY_UP:
        if (p2y1 <= windowHeight - 10) {
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
    glutInitWindowSize(1000, 600);
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
