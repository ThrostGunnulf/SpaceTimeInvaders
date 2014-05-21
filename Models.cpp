#include "Models.hxx"

////
// Model
Model::Model() : numVertex(0), numNormals(0), numTexPoints(0), numFaces(0), faces(NULL), vertex(NULL), normals(NULL), texPoints(NULL) {}

void Model::allocSpace()
{
    faces = new int* [numFaces];
    _faces = new int [9*numFaces];
    for(int i=0; i < numFaces; i++)
        faces[i] = &_faces[9*i];

    vertex = new float* [numVertex];
    _vertex = new float [3*numVertex];
    for(int i=0; i < numVertex; i++)
        vertex[i] = &_vertex[3*i];

    normals = new float* [numNormals];
    _normals = new float [3*numNormals];
    for(int i=0; i < numNormals; i++)
        normals[i] = &_normals[3*i];

    texPoints = new float* [numTexPoints];
    _texPoints = new float [2*numTexPoints];
    for(int i=0; i < numTexPoints; i++)
        texPoints[i] = &_texPoints[2*i];
}

void Model::draw()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);

    glColor3f(1.0, 0.0, 0.0);
    for(int i=0; i < numFaces; i++)
    {
        glBegin(GL_TRIANGLES);
        for(int j=0; j < 3; j++)
        {
            GLint iV, iT, iN;
            iV = faces[i][3*j];
            iT = faces[i][3*j+1];
            iN = faces[i][3*j+2];

            glTexCoord2f(texPoints[iT][0], texPoints[iT][1]);
            glNormal3f(normals[iN][0], normals[iN][1], normals[iN][2]);
            glVertex3f(vertex[iV][0], vertex[iV][1], vertex[iV][2]);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}


void Model::drawBBox(GLfloat _x, GLfloat _y, GLfloat _sx, GLfloat _sy)
{
    updateBBox(_x, _y, _sx, _sy);

    glBegin(GL_QUADS);
    {
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
        glVertex3f(x3, y3, 0);
        glVertex3f(x4, y4, 0);
    }
    glEnd();
}

void Model::assignTexture(GLuint id)
{
    texId = id;
}


void Model::createBBox(GLfloat _height, GLfloat _width)
{
    height = _height;
    width = _width;
}


void Model::updateBBox(GLfloat x, GLfloat y, GLfloat sx, GLfloat sy)
{
    x1 = x2 = (x - width / 2);
    x3 = x4 = (x + width / 2);
    y1 = y4 = (y + width / 2);
    y2 = y3 = (y - width / 2);
}


Model::~Model()
{
    delete[] _faces;
    delete[] faces;

    delete[] _vertex;
    delete[] vertex;

    delete[] _normals;
    delete[] normals;

    delete[] _texPoints;
    delete[] texPoints;
}

////
// ModelsManager
ModelsManager::ModelsManager(std::string dir, std::string modelsList)
{
    std::string fileName = dir + modelsList;

    std::ifstream file(fileName.c_str());
    if(!file)
    {
        fprintf(stderr, "File %s not found!\n", fileName.c_str());
        exit(-1);
    }

    GLfloat height, width;
    std::string token, sOpt, tOpt;
    file >> token;
    for(int i=0; !file.eof(); i++)
    {
        loadModel(dir + token + ".obj", token, i);
        file >> token;
        file >> sOpt;
        file >> tOpt;
        if(texIdMap.find(token) == texIdMap.end())
            loadTexture(dir, token, sOpt, tOpt);

        modelsVector.at(i)->assignTexture(texIdMap[token]);

        file >> height;
        file >> width;
        modelsVector.at(i)->createBBox(height, width);

        file >> token;
    }

    file.close();
}

ModelsManager::~ModelsManager()
{
    modelsVector.clear();
    modelIndexMap.clear();
    texIdMap.clear();
}

Model* ModelsManager::getModel(const std::string name)
{
    return modelsVector.at(modelIndexMap[name]);
}


void ModelsManager::loadModel(const std::string& objFile, const std::string& modelName, int objNum)
{
    Model* model = new Model();
    std::string id, token;
    std::ifstream objF(objFile.c_str());

    countObjFileSizes(objF, model);
    model->allocSpace();
    modelsVector.push_back(model);

    int nV, nT, nN, nF;
    nV = nT = nN = nF = 0;
    while(!objF.eof())
    {
        objF >> id;

        if(id.compare("v") == 0)
        {
            objF >> model->vertex[nV][0];
            objF >> model->vertex[nV][1];
            objF >> model->vertex[nV][2];
            nV++;
        }
        else if(id.compare("vt") == 0)
        {
            objF >> model->texPoints[nT][0];
            objF >> model->texPoints[nT][1];
            nT++;
        }
        else if(id.compare("vn") == 0)
        {
            objF >> model->normals[nN][0];
            objF >> model->normals[nN][1];
            objF >> model->normals[nN][2];
            nN++;
        }
        else if(id.compare("f") == 0)
        {
            for(int i=0; i < 3; i++)
            {
                objF >> token;
                sscanf(token.c_str(), "%d/%d/%d", &(model->faces[nF][3*i]), &(model->faces[nF][3*i+1]), &(model->faces[nF][3*i+2]));
                model->faces[nF][3*i]--;
                model->faces[nF][3*i+1]--;
                model->faces[nF][3*i+2]--;
            }

            nF++;
        }
        else
            objF.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    modelIndexMap[modelName] = objNum;
}

void ModelsManager::countObjFileSizes(std::ifstream& objFile, Model* count)
{
    std::string id;
    while(!objFile.eof())
    {
        objFile >> id;

        if(id.compare("v") == 0)
            count->numVertex++;
        else if(id.compare("vt") == 0)
            count->numTexPoints++;
        else if(id.compare("vn") == 0)
            count->numNormals++;
        else if(id.compare("f") == 0)
            count->numFaces++;

        objFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    objFile.clear();
    objFile.seekg(0, std::ios::beg);
}

void ModelsManager::loadTexture(std::string dir, std::string name, std::string sOpt, std::string tOpt)
{
    GLuint texId;
    RgbImage imag;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if(sOpt.compare("GL_CLAMP") == 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    if(tOpt.compare("GL_CLAMP") == 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    imag.LoadBmpFile((dir + name + ".bmp").c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
    imag.GetNumCols(),
        imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
        imag.ImageData());

    texIdMap[name] = texId;
}
