// Title Screen variables
string title;
int titleLen;
string PVP, PVE, EVE;

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
    EVE = "AI vs. AI";
}

void titleDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    background_title.display();

    // Central Guide Line
    //    glColor3f(0.5f, 0.5f, 0.5f);
    //    glRectf((windowWidth / 2.0) - 1, 0, (windowWidth / 2.0) + 1, windowHeight);
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

    // Player Vs. AI
    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 50, 175, (windowWidth / 2.0) + 50, 225);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(EVE, (windowWidth / 2.0) - 44, 190.0);

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
            glutDisplayFunc(display);
            isAI = 1;
            setGameMode(MODE_VS_AI);
            glutPostRedisplay();
        }
        else if ((x >= ((windowWidth / 2.0) - 50)) && (x <= ((windowWidth / 2.0) + 50)) && (y >= 175) && (y <= 225)) {
            cout << EVE << endl;
            init();
            glutMouseFunc(NULL);
            glutKeyboardFunc(keyboard);
            glutDisplayFunc(display);
            isAI = 2;
            setGameMode(MODE_AI_VS_AI);
            glutPostRedisplay();
        }
    }
}

// TODO (Zach): 
    // Implement basic settings screen and modifier probability settings
    // Settings screens has button to open modifer probability screen
    // Modifier probability screen displays the modifer name, a brief description, and a modifiable frequency number.

// -- Settings Screen Functions --

void settingsInit() {

}

void settingsDisplay() {

}

void settingsKeyboard(unsigned char key, int x, int y) {

}

void settingsMouse(int button, int state, int x, int y) {

}

// -- Modifier Settings Screen Functions -- 

void modifSettingsInit() {

}

void modifSettingsDisplay() {

}

void modifSettingsKeyboard(unsigned char key, int x, int y) {

}

void modifSettingsMouse(int button, int state, int x, int y) {

}