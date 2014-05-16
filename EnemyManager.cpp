#include "EnemyManager.hxx"

#define ENEMYLINES 5
#define ENEMYCOLUMNS 11

////
// EnemyManager
EnemyManager::EnemyManager(ModelsManager* _mm, GLint currY, GLfloat currS) : mm(_mm), currentY(currY), currentSpeed(currS) 
{
	enemyMatrix = new Object** [ENEMYLINES];
	_enemyMatrix = new Object* [ENEMYLINES*ENEMYCOLUMNS];
	for(int i=0; i < ENEMYLINES; i++)
	{
		enemyMatrix[i] = &_enemyMatrix[ENEMYCOLUMNS*i];
		
		for(int j=0; j < ENEMYCOLUMNS; j++)
			_enemyMatrix[ENEMYCOLUMNS*i+j] = new Object(mm->getModel("caixa"), i+10, j+10, 0);
	}
}


void EnemyManager::update()
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

