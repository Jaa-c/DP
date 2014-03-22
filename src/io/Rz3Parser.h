/* 
 * File:   Rz3Parser.h
 * Author: jaa
 *
 * Created on 17. březen 2014, 19:44
 */

#ifndef RZ3PARSER_H
#define	RZ3PARSER_H

#include "parser.h"
#include <regex>
#include <map>

class Rz3Parser {
	ImageLoader &imgLoader;
	const string calibFile;
	const string imageFile;
	const string imageFolder;
		
	void rewriteSS(std::stringstream &ss, string &line) {
		ss.str("");
		ss.clear();
		ss << line;
	}
	
public:
	Rz3Parser(ImageLoader &imgLoader, const string cf, const string imfile, const string imfolder) 
		: imgLoader(imgLoader), calibFile(cf), imageFile(imfile), imageFolder(imfolder + "/") {}
		
	/// current gcc doesn't support C++11 regex :/, so it's stupid:
	std::vector<Photo> parseFile() {
		std::vector<Photo> data;
		
		std::string line;
		std::ifstream file(imageFile.c_str());
		std::map<int, string> fileList; //array might be OK, but who knows what kind of indexes are possible
		
		while(std::getline(file, line)) {
			std::stringstream ss(line);
			int id;
			string img;
			ss >> id >> img;
			fileList.insert({id, img});
		}		
		
		std::ifstream infile(calibFile.c_str());
		while(true) {
			std::getline(infile, line);
			if(line.find("imageSequence") != string::npos) {
				break;
			}
		}
		
		int i, iSize, iStep;
		while(true) {
			std::getline(infile, line);
			if(trim(line).length() < 3) { 
				continue;
			}
			
			line = line.substr(line.find("b(") + 2);
			std::stringstream ss(line);
			ss >> i;
			ss >> iSize;
			ss >> iStep;
			break;
		}
		
		imgLoader.setExpectedCount(iSize / (float) iStep);
		
		while(true) {
			std::getline(infile, line);
			if(line.find("Camera") != string::npos) {
				break;
			}
		}
		
		while(true) {
			std::getline(infile, line);
			std::stringstream ss(trim(line));
			int tmp = -1;
			if(!(ss >> tmp)) continue;
			if(tmp == i) break;
		}
		
		int idx;
		for(; i <= iSize; i += iStep) {
			std::stringstream ss(trim(line));
			ss >> idx;
			if(i != idx) {
				throw "Something went wrong while parsing rz3 file";
			}
			
			string tmp;
			CameraPosition cam;
			std::getline(infile, line);
			rewriteSS(ss, line);
			
			ss >> tmp >> tmp >> cam.focalL; //this is so not general...
			
			std::getline(infile, line);
			rewriteSS(ss, line);
			
			ss >> tmp >> tmp >> cam.translate[0] >> cam.translate[1] >> cam.translate[2];
						
			std::getline(infile, line);
			rewriteSS(ss, line);
			
			ss >> tmp >> tmp;
			
			for(int j = 0; j < 3; j++) {
				ss >> cam.rotate[j][0];
				ss >> cam.rotate[j][1];
				ss >> cam.rotate[j][2];				
			}
			std::getline(infile, line);
			std::getline(infile, line);
			
			glm::mat3 Rx(-1, 0, 0, 0, 1, 0, 0, 0, 1); //change the sign of X
			cam.Rt = glm::mat4(Rx * glm::transpose(cam.rotate));
			cam.Rt[3][0] = cam.translate[0];
			cam.Rt[3][1] = cam.translate[1];
			cam.Rt[3][2] = cam.translate[2];
			
			//std::cout << cam.Rt << "\n";
			
			//-----------
			string name = fileList.at(i);
			
			ImageData img(name);
			if(imgLoader.loadImage(imageFolder, name, img)) {
				data.push_back(Photo(i, img.image, img.size, img.rowPadding, cam));
			}
			
			if(i > 1) break; //tmp
		}		
	
		return data;
	}
	
	
};



#endif	/* RZ3PARSER_H */

