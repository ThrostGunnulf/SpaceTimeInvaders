#include "Object.hxx"

////
// Object
Object::Object(Model* _model, GLfloat _x, GLfloat _y, GLfloat _z) : x(_x), y(_y), z(_z), sx(1), sy(1), sz(1), velX(0), velY(0), velZ(0), angle(0), dangle(0), ax(0), ay(0), az(0)
{
    model = _model;

    if(model != NULL)
    {
        width = model->width;
        height = model->height;
    }
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

//#define DRAW_BBS  //Uncomment this to display bounding boxes
#ifdef DRAW_BBS
    glPushMatrix();
    glScalef(sx, sy, sz);
    glTranslatef(0, 0, 1);
    drawBBox(x, y);
    glPopMatrix();
#endif
}

bool Object::checkCollision(Object* bullet)
{
    updateBBox();

    if(checkPointInclusion(bullet->x1, bullet->y1)
            || checkPointInclusion(bullet->x2, bullet->y2)
            || checkPointInclusion(bullet->x3, bullet->y3)
            || checkPointInclusion(bullet->x4, bullet->y4))
        return true;

    return false;
}

bool Object::checkPointInclusion(GLfloat bulletX, GLfloat bulletY)
{
    if(bulletX >= x1 && bulletX <= x3)
    {
        if(bulletY >= y2 && bulletY <= y1)
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

    width = model->width * sx;
    height = model->height * sy;
}

void Object::updateBBox()
{
    //std::cout <<"(X, Y) = ("<< x <<", "<< y <<")\n";

    x1 = x2 = x - (width / 2);
    x3 = x4 = x + (width / 2);
    y1 = y4 = y + height;
    y2 = y3 = y;
}

void Object::drawBBox(GLfloat _x, GLfloat _y)
{
    updateBBox();

    glBegin(GL_QUADS);
    {
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
        glVertex3f(x3, y3, 0);
        glVertex3f(x4, y4, 0);
    }
    glEnd();
}

GLfloat Object::getVelX()
{
    return velX;
}
