#ifndef PORTABLE_H
#define PORTABLE_H

#include <string>

#ifdef _WIN32
#include <GL/glut.h>
static std::string DIRSYMBOL = "\\";
#endif

#ifdef linux
#include <GL/freeglut.h>
static std::string DIRSYMBOL = "/";
#endif

#endif
