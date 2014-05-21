#ifndef MODELS_HXX
#define MODELS_HXX

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <limits>

#include "portable.h"
#include "RgbImage.h"

struct Model
{
    int numVertex, numNormals, numTexPoints, numFaces;
    int** faces;
    float height, width, x1, x2, x3, x4, y1, y2, y3, y4;
    float **vertex, **normals, **texPoints;

    Model();
    ~Model();
    void allocSpace();
    void draw();
    void assignTexture(GLuint id);
    void createBBox(GLfloat _height, GLfloat _width);
    void updateBBox(GLfloat x, GLfloat y, GLfloat sx, GLfloat sy);
    void drawBBox(GLfloat _x, GLfloat _y, GLfloat _sx, GLfloat _sy);

private:
    GLuint texId;
    int *_faces;
    float *_vertex, *_normals, *_texPoints;
};

class ModelsManager
{
public:
    ModelsManager(std::string dir, std::string modelsList);
    ~ModelsManager();
    Model* getModel(const std::string name);

private:
    std::vector<Model*> modelsVector;
    std::map<std::string, int> modelIndexMap;
    std::map<std::string, GLuint> texIdMap;

    void loadModel(const std::string& objFile, const std::string& modelName, int objNum);
    void countObjFileSizes(std::ifstream& objFile, Model* count);
    void loadTexture(std::string dir, std::string name, std::string sOpt, std::string tOpt);
};


#endif
