#include "portable.h"
#include "Object.hxx"
#include "main.h"

#define ENEMYLINES 5
#define ENEMYCOLUMNS 11

class EnemyManager
{
public:
    EnemyManager(ModelsManager* _mm, GLint currY, GLfloat currS, GLfloat dBetweenEnemies);
    ~EnemyManager();
	void move();
    void draw();
    bool reachedLimit(void);
    int checkCollision(Object*&);
    bool checkPointInclusion(GLfloat, GLfloat, Object*);
    void updateBBoxes();
    bool checkGameover();
    bool checkEnemyExists(int, int);
    GLfloat getEnemyX(int, int);
    GLfloat getEnemyY(int, int);

private:
	ModelsManager* mm;
    Object*** enemyMatrix;
	Object** _enemyMatrix;
    GLint currentY;
    GLfloat currentSpeed, dBetweenEnemies;
    bool goingRight;
};
