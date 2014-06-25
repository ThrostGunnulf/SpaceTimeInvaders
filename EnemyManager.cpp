#include "EnemyManager.hxx"

#define GAMEOVER_LINE 40

////
// EnemyManager
EnemyManager::EnemyManager(ModelsManager* _mm, GLint currY, GLfloat currS, GLfloat dBetweenEnemies) : mm(_mm), currentY(currY), currentSpeed(currS), goingRight(true)
{
    liveEnemies = ENEMYCOLUMNS * ENEMYLINES;

    enemyMatrix = new Object** [ENEMYLINES];
    _enemyMatrix = new Object* [ENEMYLINES*ENEMYCOLUMNS];
    for(int i=0; i < ENEMYLINES; i++)
    {
        enemyMatrix[i] = &_enemyMatrix[ENEMYCOLUMNS*i];

        for(int j=0; j < ENEMYCOLUMNS; j++)
        {
            if(i < 2)
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader3"), j*20+dBetweenEnemies-100, (i*20+dBetweenEnemies) + currentY, 0);
            else if(i < 4)
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader1"), j*20+dBetweenEnemies-100, (i*20+dBetweenEnemies) + currentY, 0);
            else
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader2"), j*20+dBetweenEnemies-100, (i*20+dBetweenEnemies) + currentY, 0);


            _enemyMatrix[ENEMYCOLUMNS*i+j]->setScale(1, 1, 10);
            _enemyMatrix[ENEMYCOLUMNS*i+j]->setVelocity(0.25, 0, 0);
        }
    }
}

EnemyManager::~EnemyManager()
{
    delete[] enemyMatrix;
    delete[] _enemyMatrix;
}

bool EnemyManager::checkEnemyExists(int line, int column)
{
    if(enemyMatrix[line][column] != NULL)
        return true;

    return false;
}

GLfloat EnemyManager::getEnemyX(int line, int column)
{
    return enemyMatrix[line][column]->x;
}

GLfloat EnemyManager::getEnemyY(int line, int column)
{
    return enemyMatrix[line][column]->y;
}

void EnemyManager::draw()
{
    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
                enemyMatrix[i][j]->update(1);
        }
    }
}

bool EnemyManager::reachedLimit()
{
    GLfloat limit, delta, stop;
    int j;

    if(goingRight)
    {
        limit = 150;
        stop = -1;
        delta = -1;
        j = ENEMYCOLUMNS -1;
    }
    else
    {
        limit = -150;
        stop = ENEMYCOLUMNS;
        delta = 1;
        j = 0;
    }

    for(; j != stop; j+=delta)
    {
        for(int i = 0; i < ENEMYLINES; i++)
        {
            if(enemyMatrix[i][j] != NULL)
            {
                if(goingRight && enemyMatrix[i][j]->x > limit)
                    return true;

                if(!goingRight && enemyMatrix[i][j]->x < limit)
                    return true;
            }
        }
    }

    return false;
}


void EnemyManager::move()
{
    if(liveEnemies == 0)
        return;

    bool finished = reachedLimit();

    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
            {
                GLfloat direction = enemyMatrix[i][j]->getVelX() > 0 ? 1 : -1;
                enemyMatrix[i][j]->setVelocity((0.25 + (ENEMYCOLUMNS*ENEMYLINES - liveEnemies) * 0.005) * direction, 0, 0);
            }
        }
    }

    if(!finished)
        return;

    goingRight = !goingRight;

    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
            {
                enemyMatrix[i][j]->translate(0, -10, 0);
                enemyMatrix[i][j]->setVelocity(enemyMatrix[i][j]->getVelX() * -1, 0, 0);
            }
        }
    }
}


int EnemyManager::checkCollision(Object*& bullet)
{
    bool answer;
    int points;

    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] == NULL)
                continue;

            enemyMatrix[i][j]->updateBBox();

            for(int l = 0; l < 4; l++)
            {
                switch(l)
                {
                case 0:
                {
                    answer = checkPointInclusion(bullet->x1, bullet->y1, enemyMatrix[i][j]);
                    break;
                }
                case 1:
                {
                    answer = checkPointInclusion(bullet->x2, bullet->y2, enemyMatrix[i][j]);
                    break;
                }
                case 2:
                {
                    answer = checkPointInclusion(bullet->x3, bullet->y3, enemyMatrix[i][j]);
                    break;
                }
                default:
                {
                    answer = checkPointInclusion(bullet->x4, bullet->y4, enemyMatrix[i][j]);
                    break;
                }
                }

                if(answer)
                {
                    destroyBullet();
                    delete enemyMatrix[i][j];
                    enemyMatrix[i][j] = NULL;
                    liveEnemies--;

                    //INSERT BIG EXPLOSION HERE

                    points = 10*(1 + i/2);

                    return points;
                }
            }
        }
    }

    return 0;
}


bool EnemyManager::checkPointInclusion(GLfloat bulletX, GLfloat bulletY, Object* enemy)
{
    if(bulletX >= enemy->x1 && bulletX <= enemy->x3)
    {
        if(bulletY >= enemy->y2 && bulletY <= enemy->y1)
            return true;
    }

    return false;
}


void EnemyManager::updateBBoxes()
{
    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
                enemyMatrix[i][j]->updateBBox();
        }
    }
}


bool EnemyManager::checkGameover()
{
    for(int i = ENEMYLINES - 1; i >= 0; i--)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
            {
                if(enemyMatrix[i][j]->y <= GAMEOVER_LINE)
                    return true;
            }
        }
    }

    return false;
}


