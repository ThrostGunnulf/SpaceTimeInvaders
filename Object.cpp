#include "Object.hxx"

////
// Object
Object::Object(Model* _model, GLfloat _x, GLfloat _y, GLfloat _z) : x(_x), y(_y), z(_z), velX(0), velY(0), velZ(0), angle(0), dangle(0), ax(0), ay(0), az(0), sx(1), sy(1), sz(1)
{
    model = _model;
}

void Object::update(GLfloat delta)
{
	angle += dangle;
	normalizeAngle();

	translate(velX, velY, velZ);

    glPushMatrix();
		glTranslatef(x, y, z);
		glRotatef(angle, ax, ay, az);
        glScalef(sx, sy, sz);
		model->draw();
    glPopMatrix();

    glPushMatrix();
    glScalef(sx, sy, sz);
    glTranslatef(0, 0, 1);
    model->drawBBox();
    glPopMatrix();
}

void Object::translate(GLfloat dx, GLfloat dy, GLfloat dz)
{
	x += dx;
	y += dy;
	z += dz;
}

void Object::setRotation(GLfloat vel, GLfloat _ax, GLfloat _ay, GLfloat _az)
{
	dangle = vel;
	ax = _ax;
	ay = _ay;
	az = _az;
}

void Object::setVelocity(GLfloat _velX, GLfloat _velY, GLfloat _velZ)
{
	velX = _velX;
	velY = _velY;
	velZ = _velZ;
}

void Object::normalizeAngle()
{
	while(angle > 360)
		angle -= 360;

	while(angle < 0)
		angle += 360;
}

void Object::setScale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ)
{
	sx = scaleX;
	sy = scaleY;
	sz = scaleZ;
}

GLfloat Object::getVelX()
{
    return velX;
}
