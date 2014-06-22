#include "Object.hxx"

////
// Object
Object::Object(Model* _model, GLfloat _x, GLfloat _y, GLfloat _z) : x(_x), y(_y), z(_z), sx(1), sy(1), sz(1), velX(0), velY(0), velZ(0), angle(0), dangle(0), ax(0), ay(0), az(0)
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

    //Uncomment this section to display bounding boxes
    /*glPushMatrix();
    glScalef(sx, sy, sz);
    glTranslatef(0, 0, 1);
    model->drawBBox(x, y, sx, sy);
    glPopMatrix();*/
}

bool Object::checkCollision(Object* bullet)
{
    model->updateBBox(x, y);

    if(checkPointInclusion(bullet->model->x1, bullet->model->y1) || checkPointInclusion(bullet->model->x2, bullet->model->y2) ||
            checkPointInclusion(bullet->model->x3, bullet->model->y3) || checkPointInclusion(bullet->model->x4, bullet->model->y4))
        return true;

    return false;
}

bool Object::checkPointInclusion(GLfloat bulletX, GLfloat bulletY)
{
    if(bulletX >= model->x1 && bulletX <= model->x3)
    {
        if(bulletY >= model-> y2 && bulletY <= model->y1)
            return true;
    }

    return false;
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
