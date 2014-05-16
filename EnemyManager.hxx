#include "portable.h"
#include "Object.hxx"

class EnemyManager
{
public:
    EnemyManager(ModelsManager* _mm, GLint currY, GLfloat currS);
    ~EnemyManager();
	void move();
	void checkCollision(GLfloat x, GLfloat y);
	void update();

private:
	ModelsManager* mm;
    Object*** enemyMatrix;
	Object** _enemyMatrix;
	GLint currentY;
	GLfloat currentSpeed;
};
