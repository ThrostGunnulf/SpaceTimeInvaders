#include <cstring>

#include "DefenseBunker.hxx"

DefenseBunker::DefenseBunker(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _width, GLfloat _height, GLfloat _depth) : Object(NULL, _x, _y, _z)
{
    texId = -1;

    width = _width;
    height = _height;
    depth = _depth;

    matrixFileName = "models" + DIRSYMBOL + "DefenseBunker.txt";
    loadIntactMatrix(matrixFileName);

    halfWidth = width * nRows / 2.0;
    halfHeight = height * nCols / 2.0;
    halfDepth = depth / 2.0;
}

void DefenseBunker::assignTexture(std::string dir, std::string name)
{
    RgbImage imag;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    imag.LoadBmpFile((dir + DIRSYMBOL + name + ".bmp").c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 imag.GetNumCols(),
                 imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imag.ImageData());
}

void DefenseBunker::update(GLfloat delta)
{
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    angle += dangle;
    normalizeAngle();

    translate(velX, velY, velZ);

    glPushMatrix();

    glTranslatef(x, y, z);
    glRotatef(angle, ax, ay, az);
    glScalef(sx, sy, sz);
    glTranslatef(-halfWidth, -halfHeight, halfDepth);

    GLfloat origX = 0, origY = 0;
    for(int i=1; i <= nRows; i++, origY += height)
    {
        origX = 0;
        for(int j=1; j <= nCols; j++, origX += width)
        {
            if(!intactAt(i, j))
                continue;

            //Draw front wall
            glBegin(GL_QUADS);
            glNormal3f(0, 0, 1);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(origX, origY + height, 0.0);

            glTexCoord2f(0.0, 0.0);
            glVertex3f(origX, origY, 0.0);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(origX + width, origY, 0.0);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(origX + width, origY + height, 0.0);
            glEnd();

            //draw back wall
            glBegin(GL_QUADS);
            glNormal3f(0, 0, -1);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(origX + width, origY + height, -depth);

            glTexCoord2f(0.0, 0.0);
            glVertex3f(origX + width, origY, -depth);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(origX, origY, -depth);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(origX, origY + height, -depth);
            glEnd();

            //Draw left wall
            if(!intactAt(i, j -1))
            {
                glBegin(GL_QUADS);
                glNormal3f(-1, 0, 0);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(origX, origY + height, -depth);

                glTexCoord2f(0.0, 0.0);
                glVertex3f(origX, origY, -depth);

                glTexCoord2f(1.0, 0.0);
                glVertex3f(origX, origY, 0);

                glTexCoord2f(1.0, 1.0);
                glVertex3f(origX, origY + height, 0);
                glEnd();
            }
            //Draw right wall
            if(!intactAt(i, j +1))
            {
                glBegin(GL_QUADS);
                glNormal3f(1, 0, 0);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(origX + width, origY + height, 0);

                glTexCoord2f(0.0, 0.0);
                glVertex3f(origX + width, origY, 0);

                glTexCoord2f(1.0, 0.0);
                glVertex3f(origX + width, origY, -depth);

                glTexCoord2f(1.0, 1.0);
                glVertex3f(origX + width, origY + height, -depth);
                glEnd();
            }

            //Draw lower wall
            if(!intactAt(i -1, j))
            {
                glBegin(GL_QUADS);
                glNormal3f(0, -1, 0);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(origX, origY, 0);

                glTexCoord2f(0.0, 0.0);
                glVertex3f(origX, origY, -depth);

                glTexCoord2f(1.0, 0.0);
                glVertex3f(origX + width, origY, -depth);

                glTexCoord2f(1.0, 1.0);
                glVertex3f(origX + width, origY, 0);
                glEnd();
            }
            //Draw upper wall
            if(!intactAt(i +1, j))
            {
                glBegin(GL_QUADS);
                glNormal3f(0, 1, 0);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(origX, origY + height, -depth);

                glTexCoord2f(0.0, 0.0);
                glVertex3f(origX, origY + height, 0);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(origX + width, origY + height, 0);

                glTexCoord2f(1.0, 1.0);
                glVertex3f(origX + width, origY + height, -depth);
                glEnd();
            }
        }
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
}


bool DefenseBunker::checkColision(Object* bullet)
{
    for(int i=1; i <= nRows; i++)
        for(int j=1; j <= nCols; j++)
            if(intactAt(i, j))
            {
                bool collision = checkPointInclusion(bullet->x1, bullet->y1, i, j)
                        || checkPointInclusion(bullet->x2, bullet->y2, i, j)
                        || checkPointInclusion(bullet->x3, bullet->y3, i, j)
                        || checkPointInclusion(bullet->x4, bullet->y4, i, j);

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
    /*for(int i=1; i <= nRows; i++)
    {
        for(int j=1; j <= nCols; j++)
            printf("%d ", intactMatrix[i * nCols + j]);
        printf("\n");
    }*/


    matrixFile.close();
}

void DefenseBunker::reset(void)
{
    loadIntactMatrix(matrixFileName);
}
