/* 
 * File:   ScannerParser.h
 * Author: jaa
 *
 * Created on 10. květen 2014, 5:23
 */

#ifndef SCANNERPARSER_H
#define	SCANNERPARSER_H

#include "parser.h"
#include "ImageBB.h"

class ScannerParser {
	ImageLoader &imgLoader;
	const string calibFile;
	const string imageFolder;
	
	void rewriteSS(std::stringstream &ss, string &line) {
		ss.str("");
		ss.clear();
		ss << line;
	}
	
public:
	ScannerParser(ImageLoader &imgLoader, const string cf, const string imfolder) 
		: imgLoader(imgLoader), calibFile(cf), imageFolder(imfolder + "/") {}
		
	/// current gcc doesn't support C++11 regex :/, so it's stupid:
	std::vector<CameraPosition> parseFile() {
		std::vector<CameraPosition> cameras;
		
		std::string line;
		std::ifstream infile(calibFile.c_str());
		while(true) {
			std::getline(infile, line);
			if(line.find("element") == string::npos) {
				break;
			}
		
			glm::vec3 up;
			CameraPosition cam;	
			std::getline(infile, line);
			line = trim(line);
			line = line.substr(line.find("- viewpoint: ") + strlen("- viewpoint: "));
			std::stringstream ss(line);
			
			ss >> cam.position.x >> cam.position.y >> cam.position.z; 
			
			std::getline(infile, line);
			line = trim(line);
			line = line.substr(line.find("- direction: ") + strlen("- direction: "));
			rewriteSS(ss, line);
			ss >> cam.direction.x >> cam.direction.y >> cam.direction.z; 
			
			std::getline(infile, line);
			line = trim(line);
			line = line.substr(line.find("- upVector : ") + strlen("- upVector : "));
			rewriteSS(ss, line);
			ss >> up.x >> up.y >> up.z;
			
			up = -up;
			glm::vec3 l = glm::normalize(glm::cross(up, cam.direction));
			
			glm::mat3 rotate(
				l.x, l.y, l.z,
				up.x, up.y, up.z,
				cam.direction.x, cam.direction.y, cam.direction.z
			);
			
			cam.focalL = 3550.0f;
			cam.Rt = glm::mat4(glm::transpose(rotate));
			glm::vec3 t = rotate * cam.position;
			cam.Rt[3][0] = -t.x;
			cam.Rt[3][1] = -t.y;
			cam.Rt[3][2] = -t.z;
			
			cameras.push_back(cam);
		}		
		
		return cameras;
	}

};


#endif	/* SCANNERPARSER_H */

