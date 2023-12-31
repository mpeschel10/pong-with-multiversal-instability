// Title Screen variables
string PVP, PVE, EVE, SET;
float titleAngle;
float r, g, b, a;
bool up, red, green, blue;

// Settings variables
bool musicOn = true;
float volume = 1.0f;
Slider volumeSlider(200.0f, 480.0f, 500.0f, 480.0f, volume);

// Modif Settings Variables
modifMenuOption modifOptions[numModifiers];
float scrollVal = 0.0f;

// -- Title Screen Functions --

void titleInit() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);

    // Enable alpha transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    PVP = "Player vs. Player";
    PVE = "Player vs. AI";
    EVE = "AI vs. AI";
    SET = "Settings";
    titleAngle = 0;
    r = 0.5f;
    g = 0.5f;
    b = 1.0f;
    a = 1.0f;
    up = false;
    red = false;
    green = false;
    blue = true;
}

void titleDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    //Background Texture
    //background_title.display();
    glColor4f(r, g, b, a);
    glTranslatef((windowWidth / 2), (windowHeight / 2), 0);
    glRotatef(titleAngle, 0.0, 0.0, 1.0);
    glTranslatef(-(windowWidth / 2), -(windowHeight / 2), 0);
    background_swirl.display();
    glTranslatef((windowWidth / 2), (windowHeight / 2), 0);
    glRotatef(-titleAngle, 0.0, 0.0, 1.0);
    glTranslatef(-(windowWidth / 2), -(windowHeight / 2), 0);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Central Guide Line
    //    glColor3f(0.5f, 0.5f, 0.5f);
    //    glRectf((windowWidth / 2.0) - 1, 0, (windowWidth / 2.0) + 1, windowHeight);
    //    glColor3f(1.0f, 1.0f, 1.0f);

    // Title Texture
    title.display();

    // Player Vs. Player
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 85, 375, (windowWidth / 2.0) + 85, 425);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(PVP, (windowWidth / 2.0) - 80, 390.0);


    // Player Vs. AI
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 70, 275, (windowWidth / 2.0) + 70, 325);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(PVE, (windowWidth / 2.0) - 63, 290.0);

    // Player Vs. AI
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
    glRectf((windowWidth / 2.0) - 50, 175, (windowWidth / 2.0) + 50, 225);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(EVE, (windowWidth / 2.0) - 44, 190.0);

    // Settings
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
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
            glutKeyboardFunc(keyboard);
            glutDisplayFunc(display);
            isAI = 1;
            setGameMode(MODE_VS_AI);
            glutPostRedisplay();
        }
        else if ((x >= ((windowWidth / 2.0) - 50)) && (x <= ((windowWidth / 2.0) + 50)) && (y >= 175) && (y <= 225)) {
            //cout << EVE << endl;
            init();
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

void titleIdle() {
    titleAngle = (titleAngle + 0.1);
    if (abs(titleAngle) >= 360) titleAngle = 0;
    glutPostRedisplay();

    if (a >= 1.0f) {
        up = false;
    }
    else if (a <= 0.7f) {
        up = true;
    }

    if (up) {
        a += 0.0005f;
    }
    else {
        a -= 0.0005f;
    }

    if (blue) {
        b -= 0.001f;
        r += 0.001f;
    }
    else if (red) {
        r -= 0.001f;
        g += 0.001f;
    }
    else if (green) {
        g -= 0.001f;
        b += 0.001f;
    }

    if (b < 0.5f) {
        blue = false;
        red = true;
    }
    else if (r < 0.5f) {
        red = false;
        green = true;
    }
    else if (g < 0.5f) {
        green = false;
        blue = true;
    }

    r = r < 0.5f ? 0.5f : r;
    g = g < 0.5f ? 0.5f : g;
    b = b < 0.5f ? 0.5f : b;

    r = r > 1.0f ? 1.0f : r;
    g = g > 1.0f ? 1.0f : g;
    b = b > 1.0f ? 1.0f : b;
}

// TODO (Zach): 
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

    if(musicOn) { glColor3f(0.0f, 1.0f, 0.0f); }
    else { glColor3f(1.0f, 0.0f, 0.0f);  }
    glRectf(215, 520, 295, 545);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText("Mute Music:", 80.0f, 525.0f);
    if(musicOn) { renderText("On", 240.0f, 525.0f, 0.0f, 0.0f, 0.0f); }
    else { renderText("Off", 240.0f, 525.0f, 0.0f, 0.0f, 0.0f); }

    renderText("Volume:", 80.0f, 475.0f);
    volumeSlider.display();

    glColor3f(0.25f, 0.25f, 0.25f);
    glRectf(80.0f, 400.0f, 230.0f, 440.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText("Modif Settings", 83.0f, 410.0f);

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
        else if ((x >= 215) && (x <= 295) && (y >= 520) && (y <= 545)) {
            if (musicOn) {
                SoundEngine->stopAllSounds();
                musicOn = false;
            }
            else {
                SoundEngine->play2D("audio/bgm.wav", true);
                musicOn = true;
            }
        }
        else if ((x >= 80) && (x <= 230) && (y >= 400) && (y <= 440)) {
            setGameMode(MODE_MODIF_SETTINGS);
            glutPostRedisplay();
        }
        else if (volumeSlider.isWithinBounds(x,y)) {
            volume = volumeSlider.setHandle(x);
            SoundEngine->setSoundVolume(volume);
            glutPostRedisplay();
        }
    }
}

// -- Modifier Settings Screen Functions -- 

void modifOptionInit() {
    for (int i = 0; i < numModifiers; i++) {
        modifOptions[i] = modifMenuOption(i, 80.0f, 525.0f - (50.0f * i));
    }
}

void modifSettingsInit() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
}

void modifSettingsDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    renderTextWarpable("Modifier Probability Settings", 50, 580);

    glColor3f(0.25f, 0.25f, 0.25f);
    glRectf(890, 570, 960, 610);
    glColor3f(1.0f, 1.0f, 1.0f);
    renderTextWarpable("Back", 900, 580);

    for (int i = 0; i < numModifiers; i++) {
        modifOptions[i].display();
    }

    glutSwapBuffers();
}

void modifSettingsKeyboard(unsigned char key, int x, int y) {
    keyboardDown[key] = true;
    switch (key) {
    case 27:
        setGameMode(MODE_SETTINGS);
        break;
    }
    glutPostRedisplay();
}

void modifSettingsMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = (windowHeight - scrollVal) - y;

        if ((x >= 890) && (x <= 960) && (y >= 570) && (y <= 610)) {
            setGameMode(MODE_SETTINGS);
            //for (int i = 0; i < numModifiers; i++) {
            //    cout << modifProbs[i] << " ";
            //}
            //cout << endl;
            glutPostRedisplay();
        }
        else {
            for (int i = 0; i < numModifiers; i++) {
                modifOptions[i].changeProb(x, y);
            }
        }
    }
    else if (state == GLUT_DOWN) {
        if (button == 3) { // Scroll Up
            scrollVal -= 10.0f;
            if (scrollVal < 0.0f) scrollVal = 0.0f;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, windowWidth, 0 - scrollVal, windowHeight - scrollVal);
            glMatrixMode(GL_MODELVIEW);
            //cout << "Scroll Up" << endl;
            glutPostRedisplay();
        }
        else if (button == 4) { // Scroll Down
            scrollVal += 10.0f;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, windowWidth, 0 - scrollVal, windowHeight - scrollVal);
            glMatrixMode(GL_MODELVIEW);
            //cout << "Scroll Down" << endl;
            glutPostRedisplay();
        }
    }
}

/*void modifSettingsScroll(int button, int dir, int x, int y) {
    if (dir > 0) {

    }
    else {

    }
}*/