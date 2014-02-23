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

#include "Log.h"

#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"

struct CameraPosition {
	glm::mat3 rotate;
	glm::vec3 translate;
	glm::mat4 Rt;
	
	float focalL;
	float d1, d2;
};

class BundlerData {
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
	BundlerData() {}
	~BundlerData() {}
	
	typedef std::vector<CameraPosition> Cameras;
	
	void parseFile(std::string file) {
		points.clear();
		cameras.clear();
		cameraDirections.clear();
		
		std::string error = "File:\n" + file + "\n doesn't seem to be valid bundler file";
		
		std::ifstream infile(file.c_str());
		std::string line;
		
		if(std::getline(infile, line)) {
			if(trim(line) != "# Bundle file v0.3") {
				Log::e("<BundlerParser> Doesn't seem to be bundler file");
				throw error;
				return;
			}
		}
		else {
			throw error;
			return;
		}
		
		std::getline(infile, line);
		std::istringstream ss(line);
		
		int c, p;
		if(!(ss >> c && ss >> p)) {
			throw error;
			return;
		}
		cameraDirections.reserve(c);
		cameras.reserve(c);
		Log::i("[BundlerData] Loading %d cameras, %d points", c, p);
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
			cameraDirections.push_back(glm::vec3(-cam.rotate[2]));
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
	
	int getClosestCamera(const glm::vec3 & dir, const glm::mat4 &mvm) const {
		glm::vec2 ndir(dir.x, dir.z);
		ndir = glm::normalize(ndir);
		const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));
		auto max = std::max_element(cameraDirections.begin(), cameraDirections.end(), 
			[ndir, vecMat] (const glm::vec3 &a, const glm::vec3 &b) -> bool {
				const glm::vec4 ta = vecMat * glm::vec4(a, 1.0f);
				const glm::vec4 tb = vecMat * glm::vec4(b, 1.0f);
				const glm::vec2 v1(ta.x, ta.z);
				const glm::vec2 v2(tb.x, tb.z);
				return glm::dot(glm::normalize(v1), ndir) < glm::dot(glm::normalize(v2), ndir);
			}
		);
		return std::distance(cameraDirections.begin(), max);
	}
	
	const std::vector<glm::vec3> &getCamerDirections() const {
		return cameraDirections;
	}
	
};

#endif	/* BUNDLERPARSER_H */
