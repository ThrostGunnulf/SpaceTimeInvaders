#include <string.h>
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

void Model::setTextureEnvironment(void)
{
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textureMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hWrap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vWrap);
}

void Model::draw()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);

    setTextureEnvironment();

    glMaterialfv(GL_FRONT, GL_AMBIENT, material->Ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material->Kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material->Ks);
    glMaterialfv(GL_FRONT, GL_SHININESS, &material->Ns);

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
    updateBBox(_x, _y);

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

void Model::assignMaterial(Material* mat)
{
    material = mat;
}

void Model::createBBox(GLfloat _height, GLfloat _width)
{
    height = _height;
    width = _width;
}

void Model::updateBBox(GLfloat x, GLfloat y)
{
    x1 = x2 = x - (width / 2);
    x3 = x4 = x + (width / 2);
    y1 = y4 = y + height;
    y2 = y3 = y;
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
// Material
Material::Material()
{
    Ns = 0;
    memset(Ka, 0, 3 * sizeof(GLfloat)); Ka[3] = 1.0;
    memset(Kd, 0, 3 * sizeof(GLfloat)); Kd[3] = 1.0;
    memset(Ks, 0, 3 * sizeof(GLfloat)); Ks[3] = 1.0;
}

void Material::setKa(GLfloat r, GLfloat g, GLfloat b)
{
    Ka[0] = r;
    Ka[1] = g;
    Ka[2] = b;
}

void Material::setKd(GLfloat r, GLfloat g, GLfloat b)
{
    Kd[0] = r;
    Kd[1] = g;
    Kd[2] = b;
}

void Material::setKs(GLfloat r, GLfloat g, GLfloat b)
{
    Ks[0] = r;
    Ks[1] = g;
    Ks[2] = b;
}

void Material::setNs(GLfloat ns)
{
    Ns = ns;
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
    std::string token, param, sOpt, tOpt;
    file >> token;
    for(int i=0; !file.eof(); i++)
    {
        loadModel(dir + token + ".obj", token, i);
        Model* curModel = modelsVector.at(i);
        file >> token;
        file >> param;
        file >> sOpt;
        file >> tOpt;
        if(texIdMap.find(token) == texIdMap.end())
            loadTexture(dir, token);

        if(param.compare("mod") == 0)
            curModel->textureMode = GL_MODULATE;
        else
            curModel->textureMode = GL_DECAL;

        if(sOpt.compare("GL_CLAMP") == 0)
            curModel->hWrap = GL_CLAMP;
        else
            curModel->hWrap = GL_REPEAT;

        if(tOpt.compare("GL_CLAMP") == 0)
            curModel->vWrap = GL_CLAMP;
        else
            curModel->vWrap = GL_REPEAT;

        curModel->assignTexture(texIdMap[token]);

        file >> width;
        file >> height;
        curModel->createBBox(height, width);

        file >> token;
        if(materialMap.find(token) == materialMap.end())
            loadMaterial(dir, token);
        curModel->assignMaterial(materialMap[token]);

        file >> token;
    }

    file.close();
}

ModelsManager::~ModelsManager()
{
    modelsVector.clear();
    modelIndexMap.clear();
    texIdMap.clear();
    materialMap.clear();
}

void ModelsManager::loadMaterial(std::string dir, std::string matName)
{
    Material* material = new Material();
    std::string fileName = dir + matName + ".mtl";
    std::string token;

    std::ifstream file(fileName.c_str());
    if(!file)
    {
        fprintf(stderr, "File %s not found!\n", fileName.c_str());
        exit(-1);
    }

    GLfloat Ka[3], Kd[3], Ks[3], Ns;
    for(int i=0; !file.eof(); i++)
    {
        file >> token;

        if(token.compare("Ka") == 0)
        {
            file >> Ka[0] >> Ka[1] >> Ka[2];
            material->setKa(Ka[0], Ka[1], Ka[2]);
        }
        else if(token.compare("Kd") == 0)
        {
            file >> Kd[0] >> Kd[1] >> Kd[2];
            material->setKd(Kd[0], Kd[1], Kd[2]);
        }
        else if(token.compare("Ks") == 0)
        {
            file >> Ks[0] >> Ks[1] >> Ks[2];
            material->setKs(Ks[0], Ks[1], Ks[2]);
        }
        else if(token.compare("Ns") == 0)
        {
            file >> Ns;
            material->setNs(Ns);
        }
        else
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    file.close();

    materialMap[matName] = material;
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

void ModelsManager::loadTexture(std::string dir, std::string name)
{
    GLuint texId;
    RgbImage imag;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    imag.LoadBmpFile((dir + name + ".bmp").c_str());
    glTexImage2D(GL_TEXTURE_2D, 0, 3,
    imag.GetNumCols(),
        imag.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
        imag.ImageData());

    texIdMap[name] = texId;
}
