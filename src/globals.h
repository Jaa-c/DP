/** @file
 * File:   globals.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 22. leden 2014, 22:28
 * Some global declarations
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

const unsigned GL_ID_NONE = (unsigned)~(unsigned(0)); //!< used as default ID for uninitialized objects

#include "glm/glm.hpp"
#include <vector>

typedef unsigned char uchar;
typedef std::vector<uchar> RGBData;
typedef glm::vec3 Point;
typedef glm::vec3 Vector;
typedef std::vector<Point> Points;
typedef std::vector<Vector> Vectors;


#if DEBUG
///check OpenGL errors
#define glCheckError() glError(__FILE__,__LINE__)
#else
#define glCheckError() 
#endif //DEBUG

void glError(const char *file, int line);

#endif	/* GLOBALS_H */

