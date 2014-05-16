#include "portable.h"
#include "Models.hxx"

class Object
{
public:
    Object(Model* _model, GLfloat _x, GLfloat _y, GLfloat _z);
    ~Object();
	void update(GLfloat delta);
	void move(GLfloat dx, GLfloat dy, GLfloat dz);
	void setRotation(GLfloat vel, GLfloat _ax, GLfloat _ay, GLfloat _az);
	void setVelocity(GLfloat _velX, GLfloat _velY, GLfloat _velZ);
	void translate(GLfloat dx, GLfloat dy, GLfloat dz);
	void setScale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

private:
    GLfloat x, y, z, velX, velY, velZ, angle, dangle, ax, ay, az, sx, sy, sz;
    Model* model;

	void normalizeAngle();
};
