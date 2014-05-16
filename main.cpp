#include <cstdio>
#include <cstdlib>
#include <string>
#include <ostream>

#include "RgbImage.h"
#include "Models.hxx"
#include "Object.hxx"
#include "EnemyManager.hxx"
#include "portable.h"

#define BLACK 0.0, 0.0, 0.0, 1.0

////
// Function forward declarations
void init(void);
void resizeWindow(GLsizei, GLsizei);
void display(void);
void Timer(int);
void keyPressEvent(unsigned char, int, int);
void keyReleaseEvent(unsigned char, int, int);
void specialKeyPressEvent(int, int, int);
void specialKeyReleaseEvent(int, int, int);

////
// Global variables.
GLint fps = 100;
GLfloat msec = 1.0/fps;
bool isOrthoProj = false;
GLfloat obsP[] = {75.0, 75.0, 75.0};
GLfloat playerHorizontalMovement = 1.0;
GLfloat xC = 100.0, yC = 100.0, zC = 200.0;
GLint screenWidth = 1024, screenHeight = 768;

bool keyState[256] = {false};
bool specialKeyState[256] = {false};

Object* player = NULL;
Object* playerBullet = NULL;
ModelsManager* modelsManager = NULL;
EnemyManager* enemyManager = NULL;

void drawObjects(void)
{
    player->update(msec);
    if(playerBullet)
        playerBullet->update(msec);
	enemyManager->update();
}

void keyOperations(void)
{
    if(keyState[32] && playerBullet == NULL) //SPACEBAR
    {
        Model* bulletModel = modelsManager->getModel("caixa");
        playerBullet = new Object(bulletModel, player->x, player->y, player->z);
        playerBullet->setVelocity(0, 1, 0);
        playerBullet->setRotation(10, 1, 1, 1);
    }

    if(specialKeyState[GLUT_KEY_LEFT])
        player->translate(-playerHorizontalMovement, 0, 0);
    if(specialKeyState[GLUT_KEY_RIGHT])
        player->translate(playerHorizontalMovement, 0, 0);
}

int main(int argc, char** argv)
{
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

    glutTimerFunc(msec, Timer, 0);
    glutMainLoop();

    return 0;
}

void init(void)
{
    glClearColor(BLACK);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    modelsManager = new ModelsManager("models" + DIRSYMBOL, "models.config");
	enemyManager = new EnemyManager(modelsManager, 10, 10);
    player = new Object(modelsManager->getModel("caixa"), 0, 0, 0);
    //player->setVelocity(0.1, 0.1, 0.1);
    player->setScale(5, 5, 5);
}

void resizeWindow(GLsizei w, GLsizei h)
{
    screenWidth = w;
    screenHeight = h;

    glutPostRedisplay();
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
        gluPerspective(90.0, screenWidth/(GLfloat)screenHeight, 0.01, zC);

    //View
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obsP[0], obsP[1], obsP[2], 0,0,0, 0, 1, 0);

    //Update and draw stuff
    keyOperations();
    drawObjects();

    //Swap Buffers
    glutSwapBuffers();
}

void Timer(int value)
{
    if(playerBullet && playerBullet->y > yC)
    {
        delete playerBullet;
        playerBullet = NULL;
    }

    glutPostRedisplay();
    glutTimerFunc(msec, Timer, 0);
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
    specialKeyState[key] = true;
}

void specialKeyReleaseEvent(int key, int x, int y)
{
    specialKeyState[key] = false;
}
