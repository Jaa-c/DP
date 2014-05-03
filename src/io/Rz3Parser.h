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

#include <QImage> //tmp

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
	std::vector<Photo> parseFile(const std::vector<glm::vec3> & vertices) {
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
			
			glm::mat3 rotate;
			
			ss >> tmp >> tmp >> cam.position[0] >> cam.position[1] >> cam.position[2];
						
			std::getline(infile, line);
			rewriteSS(ss, line);
			
			ss >> tmp >> tmp;
			
			for(int j = 0; j < 3; j++) {
				ss >> rotate[j][0];
				ss >> rotate[j][1];
				ss >> rotate[j][2];				
			}
			std::getline(infile, line);
			std::getline(infile, line);
			
			cam.Rt = glm::mat4(glm::transpose(rotate));
			glm::vec3 t = glm::transpose(rotate) * cam.position;
			cam.Rt[3][0] = -t[0];
			cam.Rt[3][1] = -t[1];
			cam.Rt[3][2] = -t[2];
			
			cam.direction = rotate[2];
			
			string name = fileList.at(i);
			
			ImageData img;
			bool ok = imgLoader.checkImage(imageFolder, name, img);
			QImage qimg((imageFolder + name).c_str());
			
			struct PointPair {
				glm::ivec2 proj;
				glm::vec3 orig;
			};
				
			std::vector<PointPair> res; //position in pixels
			for(auto &p : vertices) {
				PointPair pair;
				glm::vec4 point(p, 1);
				point = cam.Rt * point;
				point /= point.z;
				glm::ivec2 pt;
				pt.x = (int) (point.x * cam.focalL + img.size.x * .5f + .5f);
				pt.y = (int) (point.y * cam.focalL + img.size.y * .5f + .5f);
				if(pt.x >= 0 && pt.x < img.size.x && pt.y >= 0 && pt.y < img.size.y) {
					pair.orig = p;
					pair.proj = pt;
					res.push_back(pair);
//					drawPoint(pt, qimg, qRgb(0, 255, 0));
				}
			}
			
			//graham's algorithm for convex hull
			std::sort(res.begin(), res.end(), 
				[] (const PointPair& a, const PointPair& b) -> bool
				{
					if(a.proj.x == b.proj.x) return a.proj.y < b.proj.y;
					return a.proj.x < b.proj.x;
				}
			);

			std::vector<PointPair> stack, stack2;
			auto it = res.begin(), it2 = res.end();
			stack.push_back(*it++);
			stack.push_back(*it);

			stack2.push_back(*--it2);
			stack2.push_back(*--it2);

			for(uint i = 2; i < res.size(); ++i) {
				auto p = (*++it);
				while(stack.size() > 1 && orientation((*(stack.end()-2)).proj, (*(stack.end()-1)).proj, p.proj) >= 0)  {
					stack.pop_back();
				}
				stack.push_back(p);

				auto p2 = (*--it2);
				while(stack2.size() > 1 && orientation((*(stack.end()-2)).proj, (*(stack.end()-1)).proj, p2.proj) >= 0)  {
					stack2.pop_back();
				}
				stack2.push_back(p2);
			}
			res.clear();
			std::copy(stack.begin(), stack.end(), std::back_inserter(res));
			std::copy(stack2.begin(), stack2.end(), std::back_inserter(res));
			
			glm::ivec2 c(0, 0);
			glm::vec3 centroid(0, 0, 0);
			int area = 0;
			if(res.size() > 0) {
				for(uint i = 0; i < res.size(); ++i) {
					auto &pt = res[i];
					drawPoint(pt.proj, qimg, qRgb(255, 0, 0), 14);
					qimg.save((imageFolder + "0-" + QString::number(i).toStdString() + "-" + name).c_str());
					c += pt.proj;
					centroid += pt.orig;
					if(i + 1 < res.size())
						area += res[i].proj.x*res[i+1].proj.y - res[i+1].proj.x*res[i].proj.y;
					else
						area += res[i].proj.x*res[0].proj.y - res[0].proj.x*res[i].proj.y;
				}
				c /= res.size();
				centroid /= res.size();
				area /= 2.f;

				cam.fixedDirection = glm::normalize(centroid - cam.position);
			}
			else {
				//convex hull not found, for whatever reason (possibly detailed image))
				cam.fixedDirection = cam.direction;
				area = img.size.x * img.size.y;
			}
//			drawPoint(centroid, qimg, qRgb(255, 255, 255), 18);
//			glm::ivec2 c = img.size/2;
//			drawPoint(c, qimg, qRgb(255, 255, 0), 18);
//			if(name.compare("0270_1020_110_09_00_000_098345.jp") == 0)
//				qimg.save((imageFolder + "0-" + name).c_str());
			                       
		
			//-----------
			
			if(ok) {
				data.push_back(
					Photo(i, img.path, cam, 
						img.size, img.rowPadding, 
						img.thumbImage, img.thumbSize, img.thumbRowPadding));
			}
			
		}		
		
	
		return data;
	}
	
	/**
	 * >0 - left turn
	 * <0 - right turn
	 * =0 - collinear
	 */
	float orientation(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3) {
		return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
	}
	
	/**
	 * DEBUG
	 */
	void drawPoint(const glm::ivec2 &p, QImage &img, const QRgb c, const int s = 7) {
		for(int i = -s; i <= s; ++i) {
			for(int j = -s; j <= s; ++j) {
				img.setPixel(p.x + i, p.y + j, c);
			}
		}
	}
	
};



#endif	/* RZ3PARSER_H */
