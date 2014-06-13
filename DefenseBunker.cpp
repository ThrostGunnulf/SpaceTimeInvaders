#include <cstring>

#include "DefenseBunker.hxx"

DefenseBunker::DefenseBunker(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _width, GLfloat _height, GLfloat _depth) : Object(NULL, _x, _y, _z)
{
    texId = -1;

    width = _width;
    height = _height;
    depth = _depth;

    loadIntactMatrix("models" + DIRSYMBOL + "DefenseBunker.txt");

    halfWidth = width * nRows / 2.0;
    halfHeight = height * nCols / 2.0;
    halfDepth = depth / 2.0;
}

void DefenseBunker::assignTexture(std::string dir, std::string name)
{
    RgbImage imag;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    imag.LoadBmpFile((dir + name + ".bmp").c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());
}

void DefenseBunker::update(GLfloat delta)
{
    glColor3f(1.0, 0.0, 0.0);
    glDisable(GL_LIGHTING); //DEBUG
    glDisable(GL_CULL_FACE);

    angle += dangle;
    normalizeAngle();

    translate(velX, velY, velZ);

    glPushMatrix();

    glTranslatef(x, y, z);
    glRotatef(angle, ax, ay, az);
    glScalef(sx, sy, sz);
    glTranslatef(-halfWidth, -halfHeight, halfDepth);

    GLfloat origX = 0, origY;
    for(int i=1; i <= nRows; i++, origY += height)
    {
        origX = 0;
        for(int j=1; j <= nCols; j++, origX += width)
        {
            if(!intactAt(i, j))
                continue;

            glBegin(GL_QUADS);
            glVertex3f(origX, origY + height, 0.0);
            glVertex3f(origX, origY, 0.0);
            glVertex3f(origX + width, origY, 0.0);
            glVertex3f(origX + width, origY + height, 0.0);
            glEnd();

            glBegin(GL_QUADS);
            glVertex3f(origX + width, origY + height, -depth);
            glVertex3f(origX + width, origY, -depth);
            glVertex3f(origX, origY, -depth);
            glVertex3f(origX, origY + height, -depth);
            glEnd();

            //Draw left wall
            if(!intactAt(i, j -1))
            {
                glBegin(GL_QUADS);
                glVertex3f(origX, origY + height, -depth);
                glVertex3f(origX, origY, -depth);
                glVertex3f(origX, origY, 0);
                glVertex3f(origX, origY + height, 0);
                glEnd();
            }
            //Draw right wall
            if(!intactAt(i, j +1))
            {
                glBegin(GL_QUADS);
                glVertex3f(origX + width, origY + height, 0);
                glVertex3f(origX + width, origY, 0);
                glVertex3f(origX + width, origY, -depth);
                glVertex3f(origX + width, origY + height, -depth);
                glEnd();
            }

            //Draw lower wall
            if(!intactAt(i -1, j))
            {
                glBegin(GL_QUADS);
                glVertex3f(origX, origY, 0);
                glVertex3f(origX, origY, -depth);
                glVertex3f(origX + width, origY, -depth);
                glVertex3f(origX + width, origY, 0);
                glEnd();
            }
            //Draw upper wall
            if(!intactAt(i +1, j))
            {
                glBegin(GL_QUADS);
                glVertex3f(origX, origY + height, -depth);
                glVertex3f(origX, origY + height, 0);
                glVertex3f(origX + width, origY + height, 0);
                glVertex3f(origX + width, origY + height, -depth);
                glEnd();
            }
        }
    }

    glPopMatrix();

    glEnable(GL_LIGHTING); //DEBUG
}


bool DefenseBunker::checkColision(Object* bullet)
{
    for(int i=1; i <= nRows; i++)
        for(int j=1; j <= nCols; j++)
            if(intactAt(i, j))
            {
                bool collision = checkPointInclusion(bullet->model->x1, bullet->model->y1, i, j) || checkPointInclusion(bullet->model->x2, bullet->model->y2, i, j) || checkPointInclusion(bullet->model->x3, bullet->model->y3, i, j) || checkPointInclusion(bullet->model->x4, bullet->model->y4, i, j);

                if(collision)
                {
                    intactMatrix[i * nCols + j] = 0;
                    return true;
                }
            }

    return false;
}

bool DefenseBunker::checkPointInclusion(GLfloat bulletX, GLfloat bulletY, int row, int col)
{
    row--; col--;

    GLfloat x1 = x + col * width - halfWidth;
    GLfloat y1 = y + row * height - halfHeight;
    GLfloat x2 = x1 + width;
    GLfloat y2 = y1 + height;

    if((bulletX >= x1 && bulletX <= x2) && (bulletY >= y1 && bulletY <= y2))
            return true;

    return false;
}

inline bool DefenseBunker::intactAt(int row, int col)
{
    return intactMatrix[row * nCols + col] == 1 ? true : false;
}

void DefenseBunker::loadIntactMatrix(std::string dataFile)
{
    std::ifstream matrixFile(dataFile.c_str());

    matrixFile >> nRows >> nCols;

    int size = (nRows+2) * (nCols+2);
    intactMatrix = new int[size];
    memset(intactMatrix, 0, size);

    for(int i=nRows; i > 0; i--)
        for(int j=1; j <= nCols; j++)
            matrixFile >> intactMatrix[i * nCols + j];

    //DEBUG
    for(int i=1; i <= nRows; i++)
    {
        for(int j=1; j <= nCols; j++)
            printf("%d ", intactMatrix[i * nCols + j]);
        printf("\n");
    }


    matrixFile.close();
}
