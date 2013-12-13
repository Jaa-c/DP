/* 
 * File:   BundlerParser.h
 * Author: jaa
 *
 * Created on 3. říjen 2013, 12:44
 */

#ifndef BUNDLERPARSER_H
#define	BUNDLERPARSER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "glm/core/type_mat3x3.hpp"

struct CameraPosition {
	glm::mat3 rotate;
	glm::vec3 translate;
	glm::mat4 Rt;
	
	float focalL;
	float d1, d2;
};

class BundlerParser {
	std::vector<glm::vec3> points;
	std::vector<CameraPosition> cameras;
	std::vector<glm::vec3> cameraDirections;
	
	// trim from start
    static inline std::string &ltrim(std::string &s) {
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	    return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
	    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	    return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
	    return ltrim(rtrim(s));
    }
	
	
public:
	BundlerParser() {}
	~BundlerParser() {}
	
	typedef std::vector<CameraPosition> Cameras;
	
	void parseFile(std::string file) {
		
		std::ifstream infile(file.c_str());
		std::string line;
		
		if(std::getline(infile, line)) {
			if(trim(line) != "# Bundle file v0.3") {
				Log::e("<BundlerParser> Doesn't seem to be bundler file");
				return;
			}
		}
		else {
			return;
		}
		
		std::getline(infile, line);
		std::istringstream ss(line);
		
		int c, p;
		if(!(ss >> c && ss >> p)) {
			return;
		}
		cameraDirections.reserve(c);
		cameras.reserve(c);
		Log::i("[BundlerParser] Loading %d cameras, %d points", c, p);
		//parse cameras
		for(int i = 0; i < c; ++i) {
			std::getline(infile, line);
			std::istringstream ss(line);
			
			CameraPosition cam;
			ss >> cam.focalL;
			ss >> cam.d1;
			ss >> cam.d2;
			
			for(int j = 0; j < 3; j++) {
				std::getline(infile, line);
				ss.str(line);
				ss >> cam.rotate[j][0];
				ss >> cam.rotate[j][1];
				ss >> cam.rotate[j][2];				
			}
			
			std::getline(infile, line);
			ss.str(line);
			
			ss >> cam.translate[0];
			ss >> cam.translate[1];
			ss >> cam.translate[2];
			
			cam.Rt = glm::mat4(glm::transpose(cam.rotate));
			cam.Rt[3][0] = cam.translate[0];
			cam.Rt[3][1] = cam.translate[1];
			cam.Rt[3][2] = cam.translate[2];
			
			cameras.push_back(cam);
			cameraDirections.push_back(-glm::vec3(cam.rotate[0][2], cam.rotate[1][2], cam.rotate[2][2]));
		}
		
		//parse points
		for(int i = 0; i < p; ++i) {
			glm::vec3 point;
			std::getline(infile, line);
			ss.str(line);
			
			ss >> point[0];
			ss >> point[1];
			ss >> point[2];
			
			std::getline(infile, line);
			std::getline(infile, line);
			
			points.push_back(point);
		}
					
	}
	
	Cameras * getCameras() {
		return &cameras;
	}
	
	std::vector<glm::vec3> * getPoints()  {
		return &points;
	}
	
	const int getClosestCamera(const glm::vec3 & dir) const {
		auto max = std::max_element(cameraDirections.begin(), cameraDirections.end(), 
			[dir] (const glm::vec3 &a, const glm::vec3 &b) -> bool {
				return glm::dot(a, dir) < glm::dot(b, dir);
			}
		);
		return std::distance(cameraDirections.begin(), max);
	}
	
};

#endif	/* BUNDLERPARSER_H */
