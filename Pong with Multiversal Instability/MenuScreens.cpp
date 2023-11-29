// Title Screen variables
string title;
int titleLen;
string PVP, PVE, EVE, SET;

// Settings variables


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
    SET = "Settings";
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

    // Settings
    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 45, 75, (windowWidth / 2.0) + 45, 125);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(SET, (windowWidth / 2.0) - 39, 90.0);

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
            //cout << PVP << endl;
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
            //cout << PVE << endl;
            init();
            glutMouseFunc(NULL);
            glutKeyboardFunc(keyboard);
            glutDisplayFunc(display);
            isAI = 1;
            setGameMode(MODE_VS_AI);
            glutPostRedisplay();
        }
        else if ((x >= ((windowWidth / 2.0) - 50)) && (x <= ((windowWidth / 2.0) + 50)) && (y >= 175) && (y <= 225)) {
            //cout << EVE << endl;
            init();
            glutMouseFunc(NULL);
            glutKeyboardFunc(keyboard);
            glutDisplayFunc(display);
            isAI = 2;
            setGameMode(MODE_AI_VS_AI);
            glutPostRedisplay();
        }
        else if ((x >= ((windowWidth / 2.0) - 45)) && (x <= ((windowWidth / 2.0) + 45)) && (y >= 75) && (y <= 125)) {
            //cout << SET << endl;
            setGameMode(MODE_SETTINGS);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
}

void settingsDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    renderText(SET, 50, 580);

    glColor3f(0.25f, 0.25f, 0.25f);
    glRectf(890, 570, 960, 610);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText("Back", 900, 580);

    glutSwapBuffers();
}

void settingsKeyboard(unsigned char key, int x, int y) {
    keyboardDown[key] = true;
    switch (key) {
    case 27:
        setGameMode(MODE_TITLE);
        break;
    }
    glutPostRedisplay();
}

void settingsMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = windowHeight - y;

        if ((x >= 890) && (x <= 960) && (y >= 570) && (y <= 610)) {
            setGameMode(MODE_TITLE);
            glutPostRedisplay();
        }
    }
}

// -- Modifier Settings Screen Functions -- 

void modifSettingsInit() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
}

void modifSettingsDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    glutSwapBuffers();
}

void modifSettingsKeyboard(unsigned char key, int x, int y) {

}

void modifSettingsMouse(int button, int state, int x, int y) {

}