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
    float **vertex, **normals, **texPoints;

    Model();
    ~Model();
    void allocSpace();
    void draw();
    void assignTexture(GLuint id);

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
