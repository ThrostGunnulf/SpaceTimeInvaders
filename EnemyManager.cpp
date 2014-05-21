#include "EnemyManager.hxx"

#define ENEMYLINES 4
#define ENEMYCOLUMNS 10

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
            if(i == 0)
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader1"), (j*20+dBetweenEnemies) - 100, (i*20+dBetweenEnemies) - 0, 0);
            else if(i == 1)
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader2"), (j*20+dBetweenEnemies) - 100, (i*20+dBetweenEnemies) - 0, 0);
            else
                _enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("t1invader3"), (j*20+dBetweenEnemies) - 100, (i*20+dBetweenEnemies) - 0, 0);


            _enemyMatrix[ENEMYCOLUMNS*i+j]->setScale(1, 1, 1);
            _enemyMatrix[ENEMYCOLUMNS*i+j]->setVelocity(0.5, 0, 0);
        }
    }
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
        limit = 100;
        stop = -1;
        delta = -1;
        j = ENEMYCOLUMNS -1;
    }
    else
    {
        limit = -100;
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
                enemyMatrix[i][j]->translate(0, 0, 0);
            }
        }
    }
}


bool EnemyManager::checkCollision(Object*& bullet)
{
    bool answer;

    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            for(int l = 0; l < 4; l++)
            {
                if(enemyMatrix[i][j] == NULL)
                    break;

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
                    bullet = NULL;
                    enemyMatrix[i][j] = NULL;

                    //INSERT BIG EXPLOSION HERE

                    std::cout << "AI JASUS ACERTASTE NUM CHULO\n";

                    return answer;
                }
            }
        }
    }

    return false;
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
    std::cout << "INICIO\n";
    for(int i = 0; i < ENEMYLINES; i++)
    {
        for(int j = 0; j < ENEMYCOLUMNS; j++)
        {
            if(enemyMatrix[i][j] != NULL)
            {
                enemyMatrix[i][j]->model->updateBBox(enemyMatrix[i][j]->x, enemyMatrix[i][j]->y, enemyMatrix[i][j]->sx, enemyMatrix[i][j]->sy);
                printf("i:%d, j:%d, x1:%f, y1:%f\n", i, j, enemyMatrix[i][j]->model->x1, enemyMatrix[i][j]->model->y1);
            }
        }
    }
}
