#include "portable.h"
#include "Object.hxx"

class EnemyManager
{
public:
    EnemyManager(ModelsManager* _mm, GLint currY, GLfloat currS, GLfloat dBetweenEnemies);
    ~EnemyManager();
	void move();
    void draw();
    bool reachedLimit(void);
    bool checkCollision(Object*&);
    bool checkPointInclusion(GLfloat, GLfloat, Object*);
    void updateBBoxes();

private:
	ModelsManager* mm;
    Object*** enemyMatrix;
	Object** _enemyMatrix;
    GLint currentY;
    GLfloat currentSpeed, dBetweenEnemies;
    bool goingRight;
};
