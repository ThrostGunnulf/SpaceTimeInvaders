#ifndef PORTABLEGL_H
#define PORTABLEGL_H

#ifdef _WIN32
#include <GL/glut.h>
std::string dirSymbol = "\\";
#endif

#ifdef linux
#include <GL/freeglut.h>
std::string dirSymbol = "/";
#endif

#endif
