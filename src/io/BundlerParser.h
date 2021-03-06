/* 
 * File:   BundlerParser.h
 * Author: jaa
 *
 * Created on 3. říjen 2013, 12:44
 */

#ifndef BUNDLERPARSER_H
#define	BUNDLERPARSER_H

#include "parser.h"
#include <set>

class BundlerParser {
	std::vector<glm::vec3> points;
	const std::string file;
		
public:
	BundlerParser(const std::string &file) : file(file) {}
	~BundlerParser() {}
			
	std::vector<CameraPosition> parseFile() {
		points.clear();
		std::vector<CameraPosition> cameras;
		
		std::string error = "File:\n" + file + "\n doesn't seem to be valid bundler file";
		
		std::ifstream infile(file.c_str());
		std::string line;
		
		if(std::getline(infile, line)) {
			if(trim(line) != "# Bundle file v0.3") {
				Log::e("<BundlerParser> Doesn't seem to be bundler file");
				throw error;
			}
		}
		else {
			throw error;
		}
		
		std::getline(infile, line);
		std::istringstream ss(line);
		
		int c, p;
		if(!(ss >> c && ss >> p)) {
			throw error;
		}
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
			
			glm::vec3 translate;
			glm::mat3 rotate;
			
			for(int j = 0; j < 3; j++) {
				std::getline(infile, line);
				ss.str(line);
				ss >> rotate[j][0];
				ss >> rotate[j][1];
				ss >> rotate[j][2];				
			}
			
			std::getline(infile, line);
			ss.str(line);
			
			ss >> translate[0];
			ss >> translate[1];
			ss >> translate[2];
			
			cam.Rt = glm::mat4(glm::transpose(rotate));
			cam.Rt[3][0] = translate[0];
			cam.Rt[3][1] = translate[1];
			cam.Rt[3][2] = translate[2];
			cam.Rt = glm::mat4(
					-1, 0, 0, 0, 
					 0, 1, 0, 0, 
					 0, 0, 1, 0, 
					 0, 0, 0, 1) * cam.Rt; //change the sign of X
			
			cam.position = -1 * glm::transpose(rotate) * translate;
			cam.direction = -rotate[2];
			cameras.push_back(cam);
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
		return cameras;	
	}
		
	std::vector<glm::vec3> &getPoints()  {
		return points;
	}	
};

#endif	/* BUNDLERPARSER_H */
