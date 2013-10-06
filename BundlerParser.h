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

#include "lib/glm/core/type_mat3x3.hpp"

struct Point {
	float x;
	float y;
	float z;
	
	Point() {};
	Point(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	
	float & operator[] (int i) {
		switch(i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw std::out_of_range("");
		}
	}
};

struct Camera {
	glm::mat3 rotate;
	glm::vec3 translate;
	
	float focalL;
	float d1, d2;
};

class BundlerParser {
	std::vector<Point> points;
	std::vector<Camera> cameras;
	
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
	
	void parseFile(std::string file) {
		
		std::ifstream infile(file.c_str());
		std::string line;
		
		if(std::getline(infile, line)) {
			if(trim(line) != "# Bundle file v0.3") {
				std::cout << "<BundlerParser> Doesn't seem to be bundler file\n";
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
		
		std::cout << "<BundlerParser> Loading " << c << " cameras, " << p << " points\n";
		//parse cameras
		for(int i = 0; i < c; ++i) {
			std::getline(infile, line);
			std::istringstream ss(line);
			
			Camera cam;
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
			
			cameras.push_back(cam);
		}
		
		//parse points
		for(int i = 0; i < p; ++i) {
			Point point;
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
	
	std::vector<Camera> * getCameras() {
		return &cameras;
	}
	
	std::vector<Point> * getPoints()  {
		return &points;
	}
	
	
	
	
};



#endif	/* BUNDLERPARSER_H */

