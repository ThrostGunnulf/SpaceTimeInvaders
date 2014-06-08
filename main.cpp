#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ostream>

#include "RgbImage.h"
#include "Models.hxx"
#include "Object.hxx"
#include "portable.h"
#include "EnemyManager.hxx"

#define ZOOM_SPEED 1.2
#define CAMERA_ROTATION_SPEED 100.0

#define BLACK 0.0, 0.0, 0.0, 1.0

////
// Function forward declarations
void init(void);
void resizeWindow(GLsizei, GLsizei);
void display(void);
void Timer(int);
void setObsPToDefault();
void keyPressEvent(unsigned char, int, int);
void keyReleaseEvent(unsigned char, int, int);
void specialKeyPressEvent(int, int, int);
void specialKeyReleaseEvent(int, int, int);
void mouseDragEvent(int, int);
void mouseClickEvent(int, int, int, int);
void drawScore();
void destroyBullet();
void destroyObjects();
void updateObsP();

////
// Global variables.
GLfloat r;
int score = 0;
GLint fps = 60;
GLfloat msec = 1.0/fps;
bool isOrthoProj = false;
GLfloat obsP[] = {0.0, 0.0, 0.0};
GLfloat defaultObsP[] = {0.0, 50.0, 200.0};
GLfloat playerHorizontalMovement = 2.0;
GLfloat xC = 100.0, yC = 100.0, zC = 200.0;
GLint screenWidth = 1024, screenHeight = 768;
bool gameLive = true;

bool keyState[256] = {false};
bool specialKeyState[256] = {false};

Object* player = NULL;
Object* playerBullet = NULL;
Object* planet = NULL;
ModelsManager* modelsManager = NULL;
EnemyManager* enemyManager = NULL;

void drawScore()
{
    char scr[20], *aux=scr;
    sprintf(scr, "Score: %d", score);

    glColor3f(255, 255, 255);

    glRasterPos2f(-150, 115);
    while (*aux)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));
}

void drawObjects(void)
{
    glutSolidCube(25);

    player->update(msec);
    if(playerBullet)
    {
        GLfloat bulletLightPos[4] = {playerBullet->x, playerBullet->y, playerBullet->z, 1.0};
        glLightfv(GL_LIGHT7, GL_POSITION, bulletLightPos);

        playerBullet->update(msec);
    }

    planet->update(msec);
}

void drawGameover()
{
    char gameoverStr[100], *aux = gameoverStr;

    sprintf(gameoverStr, "      GAME OVER!");

    glColor3f(255, 255, 255);

    glRasterPos2f(-25, 0);
    while (*aux)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));

    sprintf(gameoverStr, "(hit Space to retry)");
    aux = gameoverStr;
    glRasterPos2f(-25, -15);
    while (*aux)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));

}

void keyOperations(void)
{
    if(keyState[' '] && playerBullet == NULL) //SPACEBAR
    {
        Model* bulletModel = modelsManager->getModel("t1playermissile");
        playerBullet = new Object(bulletModel, player->x, player->y, player->z);
        playerBullet->setVelocity(0, 1, 0);
        playerBullet->setRotation(10, 0, 1, 0);

        glEnable(GL_LIGHT7);
        GLfloat colorIntensity[4] = {255, 189, 38, 1};
        GLfloat bulletLightPos[4] = {playerBullet->x, playerBullet->y, playerBullet->z, 1.0};
        glLightfv(GL_LIGHT7,GL_POSITION,				bulletLightPos);
        glLightfv(GL_LIGHT7,GL_AMBIENT,					colorIntensity);
        glLightfv(GL_LIGHT7,GL_DIFFUSE,					colorIntensity);
        glLightfv(GL_LIGHT7,GL_SPECULAR,				colorIntensity);
        glLightf(GL_LIGHT7,GL_CONSTANT_ATTENUATION,	    1);
        glLightf(GL_LIGHT7,GL_LINEAR_ATTENUATION,		0.5);
        glLightf(GL_LIGHT7,GL_QUADRATIC_ATTENUATION,	0.5);
        glLightf(GL_LIGHT7,GL_SPOT_EXPONENT,			0.1);
    }

    if(specialKeyState[GLUT_KEY_LEFT])
        player->setVelocity(-playerHorizontalMovement, 0, 0);
    else if(specialKeyState[GLUT_KEY_RIGHT])
        player->setVelocity(playerHorizontalMovement, 0, 0);

    if((!specialKeyState[GLUT_KEY_LEFT] && !specialKeyState[GLUT_KEY_RIGHT]) ||
            (specialKeyState[GLUT_KEY_LEFT] && specialKeyState[GLUT_KEY_RIGHT]))
        player->setVelocity(0, 0, 0);
}

void destroyBullet()
{
    glDisable(GL_LIGHT7);

    std::cout << "DEBUG: Bullet destroyed\n";
    delete playerBullet;
    playerBullet = NULL;
}

void Timer(int value)
{
    if(gameLive)
    {
        keyOperations();

        enemyManager->move();
        enemyManager->updateBBoxes();

        if(playerBullet != NULL)
        {
            playerBullet->model->updateBBox(playerBullet->x, playerBullet->y);
            Object* tempBullet = playerBullet;
            score += enemyManager->checkCollision(tempBullet);
        }

        if(playerBullet != NULL && playerBullet->y > 1.2*yC)
            destroyBullet();

        if(gameLive && enemyManager->checkGameover())
            gameLive = false;
    }

    if(!gameLive && keyState[' '])
    {
        gameLive = true;
        if(playerBullet != NULL)
        {
            glDisable(GL_LIGHT7);
            playerBullet = NULL;
        }

        enemyManager = new EnemyManager(modelsManager, 50, 0.25, 20);
        player = new Object(modelsManager->getModel("t1player"), 0, 0, 0);
        player->setScale(1, 1, 1);
        score = 0;
    }


    glutPostRedisplay();
    glutTimerFunc(msec, Timer, 0);
}

int main(int argc, char** argv)
{
    printf("GL_MAX_LIGHTS = %d\n(please remove me later, I'm in main)\n", (int) GL_MAX_LIGHTS);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(448, 156);
    glutCreateWindow("SpaceTime Invaders");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(resizeWindow);

    glutKeyboardFunc(keyPressEvent);
    glutKeyboardUpFunc(keyReleaseEvent);

    glutSpecialFunc(specialKeyPressEvent);
    glutSpecialUpFunc(specialKeyReleaseEvent);

    glutMouseFunc(mouseClickEvent);
    glutMotionFunc(mouseDragEvent);

    glutTimerFunc(msec, Timer, 0);
    glutMainLoop();

    destroyObjects();

    return 0;
}

void init(void)
{
    setObsPToDefault();

    glClearColor(BLACK);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    GLfloat intensidadeCor[4] = {1.0, 1.0, 1.0, 1.0};
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, intensidadeCor);

    glEnable(GL_LIGHT1);
    GLfloat direccao[4] = {0, 0, -1};
    glLightfv(GL_LIGHT1,GL_POSITION,				obsP);
    glLightfv(GL_LIGHT1,GL_AMBIENT,					intensidadeCor);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,					intensidadeCor);
    glLightfv(GL_LIGHT1,GL_SPECULAR,				intensidadeCor);
    glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,	    1.0);
    glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,		0.05);
    glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,	0.0);
    //glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,				70);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,			direccao);
    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,			2.0);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    modelsManager = new ModelsManager("models" + DIRSYMBOL, "models.config");
    enemyManager = new EnemyManager(modelsManager, 50, 0.25, 20);
    player = new Object(modelsManager->getModel("t1player"), 0, 0, 0);
    //player->setVelocity(0.1, 0.1, 0.1);
    player->setScale(1, 1, 1);
    planet = new Object(modelsManager->getModel("planet"), 275, -200, -575);
    planet->setScale(50, 50, 50);
    planet->setRotation(0.05, 0, 1, 0);
}

void display(void)
{
    //Clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, screenWidth, screenHeight);

    //Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(isOrthoProj)
        glOrtho(-xC, xC, -yC, yC, -zC, zC);
    else
        gluPerspective(90.0, screenWidth/(GLfloat)screenHeight, 0.01, 10*zC);

    //View
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obsP[0], obsP[1], obsP[2], 0,0,0, 0, 1, 0);

    //Update and draw stuff
    if(gameLive)
    {
        drawObjects();
        enemyManager->draw();
    }
    drawScore();

    if(!gameLive)
        drawGameover();

    //Swap Buffers
    glutSwapBuffers();
    glutPostRedisplay();
}

void destroyObjects()
{
    delete player;
    delete playerBullet;
    delete modelsManager;
    delete enemyManager;
}


////
// GLUT input events handlers
void resizeWindow(GLsizei w, GLsizei h)
{
    screenWidth = w;
    screenHeight = h;

    glutPostRedisplay();
}

void keyPressEvent(unsigned char key, int x, int y)
{
    keyState[key] = true;
}

void keyReleaseEvent(unsigned char key, int x, int y)
{
    keyState[key] = false;
}

void specialKeyPressEvent(int key, int x, int y)
{
    if(key > 255)
        std::cout << "ERROR: Invalid key event\n";
    else
        specialKeyState[key] = true;
}

void specialKeyReleaseEvent(int key, int x, int y)
{
    if(key <= 255)
        specialKeyState[key] = false;
}

bool rightButtonIsPressed = false;
int prevX = -1, prevY = -1;
GLfloat angleX0Z = 0, angleX0Y = 0;
void mouseDragEvent(int x, int y)
{
    if(!rightButtonIsPressed)
        return;

    if(prevX != -1)
    {
        int dX, dY;
        dX = x - prevX;
        dY = y - prevY;

        angleX0Z += dX / CAMERA_ROTATION_SPEED;
        angleX0Y += dY / CAMERA_ROTATION_SPEED;

        updateObsP();
    }

    prevX = x;
    prevY = y;
}

void mouseClickEvent(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            prevX = x;
            prevY = y;

            rightButtonIsPressed = true;
        }
        else if(state == GLUT_UP)
            rightButtonIsPressed = false;
    }
    else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        setObsPToDefault();
    else if(button == 3) //Scroll wheel forwards
    {
        if(state == GLUT_UP)
            return;

        r /= ZOOM_SPEED;
        updateObsP();
    }
    else if(button == 4) //Scroll wheel backwards
    {
        if(state == GLUT_UP)
            return;

        r *= ZOOM_SPEED;
        updateObsP();
    }
}

void updateObsP()
{
    obsP[0] = r * sin(angleX0Z);
    obsP[1] = r * sin(angleX0Y);
    obsP[2] = r * cos(angleX0Z) * cos(angleX0Y);
}

void setObsPToDefault()
{
    obsP[0] = defaultObsP[0];
    obsP[1] = defaultObsP[1];
    obsP[2] = defaultObsP[2];

    r = obsP[2];
    angleX0Z = angleX0Y = 0;
}
