#include "EnemyManager.hxx"

#define ENEMYLINES 5
#define ENEMYCOLUMNS 11

////
// EnemyManager
EnemyManager::EnemyManager(ModelsManager* _mm, GLint currY, GLfloat currS, GLfloat dBetweenEnemies) : mm(_mm), currentY(currY), currentSpeed(currS), goingRight(true)
{
    enemyMatrix = new Object** [ENEMYLINES];
    _enemyMatrix = new Object* [ENEMYLINES*ENEMYCOLUMNS];
    for(int i=0; i < ENEMYLINES; i++)
    {
        enemyMatrix[i] = &_enemyMatrix[ENEMYCOLUMNS*i];

        for(int j=0; j < ENEMYCOLUMNS; j++)
        {
            if(i < 2)
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader3"), j*20+dBetweenEnemies, (i*20+dBetweenEnemies) + currentY, 0);
            else if(i < 4)
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader1"), j*20+dBetweenEnemies, (i*20+dBetweenEnemies) + currentY, 0);
            else
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader2"), j*20+dBetweenEnemies, (i*20+dBetweenEnemies) + currentY, 0);


            _enemyMatrix[ENEMYCOLUMNS*i+j]->setScale(1, 1, 1);
            _enemyMatrix[ENEMYCOLUMNS*i+j]->setVelocity(currentSpeed, 0, 0);
        }
    }
}

EnemyManager::~EnemyManager()
{
    delete enemyMatrix;
    delete _enemyMatrix;
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
    bool finished = reachedLimit();

    if(!finished)
        return;

    goingRight = !goingRight;

    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
            {
                GLfloat currentSpeed = enemyMatrix[i][j]->getVelX();

                enemyMatrix[i][j]->setVelocity((currentSpeed+0.05)*-1, 0, 0);
                enemyMatrix[i][j]->translate(0, -10, 0);
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

            enemyMatrix[i][j]->model->updateBBox(enemyMatrix[i][j]->x, enemyMatrix[i][j]->y, enemyMatrix[i][j]->sx, enemyMatrix[i][j]->sy);

            for(int l = 0; l < 4; l++)
            {
                switch(l)
                {
                case 0:
                {
                    answer = checkPointInclusion(bullet->model->x1, bullet->model->y1, enemyMatrix[i][j]);
                    break;
                }
                case 1:
                {
                    answer = checkPointInclusion(bullet->model->x2, bullet->model->y2, enemyMatrix[i][j]);
                    break;
                }
                case 2:
                {
                    answer = checkPointInclusion(bullet->model->x3, bullet->model->y3, enemyMatrix[i][j]);
                    break;
                }
                default:
                {
                    answer = checkPointInclusion(bullet->model->x4, bullet->model->y4, enemyMatrix[i][j]);
                    break;
                }
                }

                if(answer)
                {
                    destroyBullet();
                    enemyMatrix[i][j] = NULL;

                    //INSERT BIG EXPLOSION HERE

                    points = 10*(1 + i/2);

                    std::cout << "DEBUG: Enemy hit, worth " << points << " points\n";

                    return points;
                }
            }
        }
    }

    return 0;
}


bool EnemyManager::checkPointInclusion(GLfloat bulletX, GLfloat bulletY, Object* enemy)
{
    if(bulletX >= enemy->model->x1 && bulletX <= enemy->model->x3)
    {
        if(bulletY >= enemy->model-> y2 && bulletY <= enemy->model->y1)
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
                enemyMatrix[i][j]->model->updateBBox(enemyMatrix[i][j]->x, enemyMatrix[i][j]->y, enemyMatrix[i][j]->sx, enemyMatrix[i][j]->sy);
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
                if(enemyMatrix[i][j]->y <= 20)
                    return true;
            }
        }
    }

    return false;
}


