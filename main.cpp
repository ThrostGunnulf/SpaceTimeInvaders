#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ostream>
#include <time.h>

#include "RgbImage.h"
#include "Models.hxx"
#include "Object.hxx"
#include "portable.h"
#include "EnemyManager.hxx"
#include "DefenseBunker.hxx"

#define ZOOM_SPEED 1.2
#define CAMERA_ROTATION_SPEED 100.0

#define NUM_BUNKERS 3
#define NUM_LASERS 1
#define SHOT_FREQUENCY_MAX 10000000
#define SHOT_FREQUENCY_MIN 1000

#define PLAYER_LASER_CEILING 215
#define ENEMY_LASER_FLOOR -100

#define LEFT_LIMIT -150
#define RIGHT_LIMIT 150

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
void drawScore(int, int);
void destroyBullet();
void destroyObjects();
void drawHUD();
void updateObsP();
void moveEnemyShots();
bool checkEnemyShotCollision(int);
void destroyLaser(int);
void getRandomEnemy(GLfloat*);

////
// Global variables.
GLfloat r;
int score = 0;
GLint fps = 60;
GLfloat msec = 1.0/fps;
bool isOrthoProj = false;
GLfloat obsP[] = {0.0, 50.0, 150.0};
GLfloat defaultObsP[] = {0.0, 75.0, 150.0};
GLfloat playerHorizontalMovement = 2.0;
GLfloat xC = 100.0, yC = 100.0, zC = 200.0;
GLint screenWidth = 1024, screenHeight = 768;
bool gameLive = true;

bool keyState[256] = {false};
bool specialKeyState[256] = {false};

int playerLives = 3;

Object* player = NULL;
Object* playerBullet = NULL;
Object* planet = NULL;
Object* space = NULL;
Object* enemyLasers[NUM_LASERS] = { NULL };
unsigned int firstEnemyLight = GL_LIGHT5;
GLint liveShots = 0;
ModelsManager* modelsManager = NULL;
EnemyManager* enemyManager = NULL;
DefenseBunker* bunkers[NUM_BUNKERS] = { NULL };

void drawScore(int x, int y)
{
    char scr[20], *aux=scr;
    sprintf(scr, "Score: %d", score);

    glColor3f(1.0, 1.0, 1.0);

    glRasterPos2f(x, y);
    while (*aux)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));
}

void drawSkyBox(void)
{
    glDisable(GL_CULL_FACE);
    space->setRotation(-0.01, 1, 1, 0);
    space->update(msec);
    glEnable(GL_CULL_FACE);
}

void drawObjects(void)
{
    player->update(msec);
    if(playerBullet)
    {
        GLfloat bulletLightPos[4] = {playerBullet->x, playerBullet->y, playerBullet->z + 15, 1.0};
        glLightfv(GL_LIGHT4, GL_POSITION, bulletLightPos);

        playerBullet->update(msec);
    }

    planet->update(msec);

    for(int i = 0; i < NUM_BUNKERS; i++)
        bunkers[i]->update(msec);

    for(int i = 0; i < NUM_LASERS; i++)
    {
        if(enemyLasers[i] != NULL)
        {
            GLfloat enemyLightPos[4] = {enemyLasers[i]->x, enemyLasers[i]->y, enemyLasers[i]->z + 15, 1.0};
            glLightfv(firstEnemyLight+i, GL_POSITION, enemyLightPos);

            enemyLasers[i]->update(msec);
            enemyLasers[i]->model->updateBBox(enemyLasers[i]->x, enemyLasers[i]->y);

            //Uncomment this section to display laser numbers
            /*glDisable(GL_LIGHTING);
            char scr[20], *aux=scr;
            sprintf(scr, "%d", i);

            glColor3f(0.0, 1.0, 0.0);

            glRasterPos3f(enemyLasers[i]->x, enemyLasers[i]->y, 2);
            while (*aux)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));
            glEnable(GL_LIGHTING);*/
        }
    }
}

void drawHUD()
{
    glDisable(GL_LIGHTING);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, screenWidth, screenHeight, 0.0, -1.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawScore(40, 50);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

void drawGameover()
{
    glDisable(GL_LIGHTING);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, screenWidth, screenHeight, 0.0, -1.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    char gameoverStr[100], *aux = gameoverStr;

    sprintf(gameoverStr, "GAME OVER!");

    glColor3f(255, 255, 255);

    glRasterPos2f(screenWidth/2 - 95, screenHeight/2);
    while (*aux)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));

    sprintf(gameoverStr, "(hit Space to retry)");
    aux = gameoverStr;
    glRasterPos2f(screenWidth/2 - 100, screenHeight/2 + 20);
    while (*aux)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(aux++));

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
}

void getRandomEnemy(GLfloat* coords)
{
    bool found = false;
    int line, col;

    while(!found)
    {
        line = rand() % ENEMYLINES;
        col = rand() % ENEMYCOLUMNS;

        found = enemyManager->checkEnemyExists(line, col);
    }

    coords[0] = enemyManager->getEnemyX(line, col);
    coords[1] = enemyManager->getEnemyY(line, col);
    coords[2] = 0;
}

void keyOperations(void)
{
    if(keyState[' '] && playerBullet == NULL) //SPACEBAR
    {
        Model* bulletModel = modelsManager->getModel("t1playermissile");
        playerBullet = new Object(bulletModel, player->x, player->y, player->z);
        playerBullet->setVelocity(0, 1, 0);
        playerBullet->setRotation(10, 0, 1, 0);
        glEnable(GL_LIGHT4);
        GLfloat colorIntensity[4] = {255, 189, 38, 1};
        GLfloat bulletLightPos[4] = {playerBullet->x, playerBullet->y, playerBullet->z, 1.0};
        glLightfv(GL_LIGHT4,GL_POSITION,				bulletLightPos);
        glLightfv(GL_LIGHT4,GL_AMBIENT,					colorIntensity);
        glLightfv(GL_LIGHT4,GL_DIFFUSE,					colorIntensity);
        glLightfv(GL_LIGHT4,GL_SPECULAR,				colorIntensity);
        glLightf(GL_LIGHT4,GL_CONSTANT_ATTENUATION,	    1);
        glLightf(GL_LIGHT4,GL_LINEAR_ATTENUATION,		0.5);
        glLightf(GL_LIGHT4,GL_QUADRATIC_ATTENUATION,	0.5);
        glLightf(GL_LIGHT4,GL_SPOT_EXPONENT,			0.1);
    }

    if(specialKeyState[GLUT_KEY_LEFT])
        player->setVelocity(-playerHorizontalMovement, 0, 0);
    else if(specialKeyState[GLUT_KEY_RIGHT])
        player->setVelocity(playerHorizontalMovement, 0, 0);

    if(player->x < LEFT_LIMIT)
        player->x = LEFT_LIMIT;
    else if(player->x > RIGHT_LIMIT)
        player->x = RIGHT_LIMIT;

    if((!specialKeyState[GLUT_KEY_LEFT] && !specialKeyState[GLUT_KEY_RIGHT]) ||
            (specialKeyState[GLUT_KEY_LEFT] && specialKeyState[GLUT_KEY_RIGHT]))
        player->setVelocity(0, 0, 0);
}

void destroyBullet()
{
    glDisable(GL_LIGHT4);
    delete playerBullet;
    playerBullet = NULL;
}

void destroyLaser(int laserIndex)
{
    glDisable(firstEnemyLight+laserIndex);
    delete enemyLasers[laserIndex];
    enemyLasers[laserIndex] = NULL;
    liveShots--;
    printf("LASER %d DESTRUIDO\n", laserIndex);
}


void Timer(int value)
{
    if(gameLive)
    {
        keyOperations();

        enemyManager->move();
        enemyManager->updateBBoxes();

        //printf("TESTING COLLISIONS\n");
        for(int i = 0; i < NUM_LASERS; i++)
        {
            //printf("[%d] %p\n", i, enemyLasers[i]);

            if(enemyLasers[i] == NULL)
                continue;

            if(enemyLasers[i]->y < ENEMY_LASER_FLOOR)
                destroyLaser(i);

            for(int j = 0; j < NUM_BUNKERS; j++)
            {
                if(enemyLasers[i] != NULL && bunkers[j]->checkColision(enemyLasers[i]))
                {
                    destroyLaser(i);
                    break;
                }
            }

            if(enemyLasers[i] != NULL && player->checkCollision(enemyLasers[i]))
            {
                playerLives--;
                std::cout << "PLAYER MORREU! FICOU COM " << playerLives << " vidas.\n";
                destroyLaser(i);
            }
        }
        //printf("\n");

        if(playerBullet != NULL)
        {
            playerBullet->model->updateBBox(playerBullet->x, playerBullet->y);
            Object* tempBullet = playerBullet;
            score += enemyManager->checkCollision(tempBullet);
        }

        for(int j = 0; j < NUM_BUNKERS; j++)
        {
            if(playerBullet && bunkers[j]->checkColision(playerBullet))
                destroyBullet();
        }

        if(playerBullet != NULL && playerBullet->y > PLAYER_LASER_CEILING)
            destroyBullet();

        if(gameLive && enemyManager->checkGameover())
            gameLive = false;

        int rng = rand() % SHOT_FREQUENCY_MAX;
        if(liveShots < NUM_LASERS && rng < SHOT_FREQUENCY_MIN)
        {
            int i;
            for(i = 0; i < NUM_LASERS; i++)
            {
                if(enemyLasers[i] == NULL)
                    break;
            }

            if(i < NUM_LASERS)
            {
                GLfloat* laserPosition = new GLfloat[3];

                getRandomEnemy(laserPosition);

                printf("NOVO LASER %d\n", i);
                enemyLasers[i] = new Object(modelsManager->getModel("t1invaderlaser"), laserPosition[0], laserPosition[1], laserPosition[2]);
                enemyLasers[i]->setVelocity(0, -1, 0);
                liveShots++;

                glEnable(firstEnemyLight+i);
                GLfloat enemyLaserIntensity[4] = {255, 0, 0, 1};
                GLfloat enemyLightPos[4] = {enemyLasers[i]->x, enemyLasers[i]->y, enemyLasers[i]->z, 1.0};
                glLightfv(firstEnemyLight+i,GL_POSITION,				enemyLightPos);
                glLightfv(firstEnemyLight+i,GL_AMBIENT,					enemyLaserIntensity);
                glLightfv(firstEnemyLight+i,GL_DIFFUSE,					enemyLaserIntensity);
                glLightfv(firstEnemyLight+i,GL_SPECULAR,				enemyLaserIntensity);
                glLightf(firstEnemyLight+i,GL_CONSTANT_ATTENUATION,	    1);
                glLightf(firstEnemyLight+i,GL_LINEAR_ATTENUATION,		0.5);
                glLightf(firstEnemyLight+i,GL_QUADRATIC_ATTENUATION,	0.5);
                glLightf(firstEnemyLight+i,GL_SPOT_EXPONENT,			0.1);
            }
        }
    }

    if(!gameLive && keyState[' '])
    {
        keyState[' '] = false;

        gameLive = true;
        if(playerBullet != NULL)
        {
            glDisable(GL_LIGHT4);
            playerBullet = NULL;
        }

        if(enemyManager != NULL)
            delete enemyManager;
        enemyManager = new EnemyManager(modelsManager, 100, 0.25, 20);
        if(player != NULL)
            delete player;
        player = new Object(modelsManager->getModel("t1player"), 0, 0, 0);
        player->setScale(1, 1, 1);
        score = 0;

        liveShots = 0;
        for(int i = 0; i < NUM_LASERS; i++)
        {
            delete enemyLasers[i];
            enemyLasers[i] = NULL;
        }

        for(int i = 0; i < NUM_BUNKERS; i++)
            bunkers[i]->reset();
    }

    glutPostRedisplay();
    glutTimerFunc(msec, Timer, 0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(448, 156);
    glutCreateWindow("SpaceTime Invaders");

    srand(time(NULL));

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
    glEnable(GL_NORMALIZE);
    //glEnable(GL_LIGHT0);

    glEnable(GL_LIGHT1);
    GLfloat intensidadeCor[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat direccao[] = {0, 5, -10, 0};
    GLfloat posicaoLuz[] = {0, 5, 3, 1};
    glLightfv(GL_LIGHT1,GL_POSITION,				posicaoLuz);
    glLightfv(GL_LIGHT1,GL_AMBIENT,					intensidadeCor);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,					intensidadeCor);
    glLightfv(GL_LIGHT1,GL_SPECULAR,				intensidadeCor);
    glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,	    1.0);
    glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,		0.0);
    glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,	0.0);
    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,				16);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,			direccao);
    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,			32.0);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    modelsManager = new ModelsManager("models" + DIRSYMBOL, "models.config");
    enemyManager = new EnemyManager(modelsManager, 100, 0.25, 20);
    player = new Object(modelsManager->getModel("t1player"), 0, 0, 0);
    //player->setVelocity(0.1, 0.1, 0.1);
    player->setScale(1, 1, 1);
    planet = new Object(modelsManager->getModel("planet"), 275, -200, -575);
    planet->setScale(50, 50, 50);
    planet->setRotation(0.05, 0, 1, 0);
    space = new Object(modelsManager->getModel("skybox"), 0, 0, 0);
    space->setScale(1250, 1250, 1250);

    bunkers[0] = new DefenseBunker(-85, 30, 0, 3, 3, 12);
    bunkers[1] = new DefenseBunker(0, 30, 0, 3, 3, 12);
    bunkers[2] = new DefenseBunker(85, 30, 0, 3, 3, 12);


    for(int i=0; i < NUM_LASERS; i++)
        enemyLasers[i] = NULL;
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
        gluPerspective(90.0, screenWidth/(GLfloat)screenHeight, 0.01, 20*zC);

    //View
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(obsP[0], obsP[1], obsP[2], 0,defaultObsP[1],0, 0, 1, 0);

    //Update and draw stuff
    if(gameLive)
    {
        drawObjects();
        enemyManager->draw();
        drawSkyBox();
        drawHUD();
    }

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
    else if(button == 4 && r < 1230) //Scroll wheel backwards
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
    obsP[1] = defaultObsP[1] + r * sin(angleX0Y);
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
