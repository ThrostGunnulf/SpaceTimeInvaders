#ifndef OBJECT_HXX
#define OBJECT_HXX

#include "portable.h"
#include "Models.hxx"

class Object
{
public:
    GLfloat x, y, z, sx, sy, sz;
    GLfloat width, height, x1, x2, x3, x4, y1, y2, y3, y4;
    Model* model;
    Object(Model* _model, GLfloat _x, GLfloat _y, GLfloat _z);
    virtual ~Object() {}
	void update(GLfloat delta);
	void move(GLfloat dx, GLfloat dy, GLfloat dz);
	void setRotation(GLfloat vel, GLfloat _ax, GLfloat _ay, GLfloat _az);
	void setVelocity(GLfloat _velX, GLfloat _velY, GLfloat _velZ);
	void translate(GLfloat dx, GLfloat dy, GLfloat dz);
	void setScale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);
    GLfloat getVelX();
    virtual bool checkCollision(Object*);
    virtual bool checkPointInclusion(GLfloat, GLfloat);
    void updateBBox();
    void drawBBox(GLfloat _x, GLfloat _y);

protected:
    GLfloat velX, velY, velZ, angle, dangle, ax, ay, az;

	void normalizeAngle();
};

#endif
