#include "Object.hxx"

class DefenseBunker : Object
{
    GLuint texId;
    int* intactMatrix;

    int nRows, nCols;
    GLfloat width, height, depth;
    GLfloat halfWidth, halfHeight, halfDepth;

public:
    DefenseBunker(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _width, GLfloat _height, GLfloat _depth);
    ~DefenseBunker() {delete intactMatrix;}

    void assignTexture(std::string dir, std::string name);
    void update(GLfloat delta);
    bool checkColision(Object* bullet);
    bool checkPointInclusion(GLfloat bulletX, GLfloat bulletY, int row, int col);
    void reset(void);

private:
    std::string matrixFileName;

    inline bool intactAt(int row, int col);
    void loadIntactMatrix(std::string dataFile);
};
