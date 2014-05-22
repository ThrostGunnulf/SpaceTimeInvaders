#include "portable.h"
#include "Object.hxx"
#include "main.h"

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

private:
	ModelsManager* mm;
    Object*** enemyMatrix;
	Object** _enemyMatrix;
    GLint currentY;
    GLfloat currentSpeed, dBetweenEnemies;
    bool goingRight;
};
