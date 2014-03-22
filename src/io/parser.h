/* 
 * File:   parser.h
 * Author: jaa
 *
 * Created on 17. březen 2014, 20:12
 */

#ifndef PARSER_H
#define	PARSER_H



#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "../Log.h"
#include "../TextureHandler.h"

#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"

using std::string;
using std::vector;

// trim from start
static inline string &ltrim(string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline string &rtrim(string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline string &trim(string &s) {
	return ltrim(rtrim(s));
}


static std::ostream & operator<<(std::ostream &os, const glm::mat3 &m) {
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			os << m[i][j] << " ";
		}
		os << "\n";
	}
	return os;
}

static std::ostream & operator<<(std::ostream &os, const glm::mat4 &m) {
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			os << m[i][j] << " ";
		}
		os << "\n";
	}
	return os;
}
#endif	/* PARSER_H */

