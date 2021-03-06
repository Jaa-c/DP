/* 
 * File:   globals.h
 * Author: jaa
 *
 * Created on 22. leden 2014, 22:28
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

const unsigned GL_ID_NONE = (unsigned)~(unsigned(0));

#include "glm/glm.hpp"
#include <vector>

typedef unsigned char uchar;
typedef std::vector<uchar> RGBData;
typedef glm::vec3 Point;
typedef glm::vec3 Vector;
typedef std::vector<Point> Points;
typedef std::vector<Vector> Vectors;

#define glCheckError() glError(__FILE__,__LINE__)
void glError(const char *file, int line);

#endif	/* GLOBALS_H */

