#include <cstdio>
#include <cstdlib>
#include <string>

#include "RgbImage.h"
#include "Models.hxx"
#include "Object.hxx"
#include "portable.h"

#define BLACK    0.0, 0.0, 0.0, 1.0

void init(void);
void resizeWindow(GLsizei, GLsizei);
void display(void);

bool isOrthoProj = false;
GLfloat obsP[] = {75.0, 75.0, 75.0};
GLfloat xC = 100.0, yC = 100.0, zC = 200.0;
GLint screenWidth = 1024, screenHeight = 768;

ModelsManager* modelsManager;
Object* invader;

void drawObjects(void)
{
    invader->update(1);
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
    //glutKeyboardFunc(keyboard);
    //glutSpecialFunc(teclasNotAscii);

    glutMainLoop();

    return 0;
}

void init(void)
{
    glClearColor(BLACK);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    modelsManager = new ModelsManager("models" + DIRSYMBOL, "models.config");
    invader = new Object(modelsManager->getModel("caixa"), 0, 0, 0);
    invader->setScale(5, 5, 5);
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

    drawObjects();

    //Swap Buffers
    glutSwapBuffers();
}