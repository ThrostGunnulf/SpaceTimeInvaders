#ifndef PORTABLEGL_H
#define PORTABLEGL_H

#ifdef _WIN32
#include <GL/glut.h>
#endif

#ifdef linux
#include <GL/freeglut.h>
#endif

#endif
