/* 
 * File:   ImageBB.h
 * Author: jaa
 *
 * Created on 3. květen 2014, 23:21
 */

#ifndef IMAGEBB_H
#define	IMAGEBB_H


#include <QImage>

#include "ImageLoader.h" //tmp

/**
 * Computes Bounding Box in image space to find center and area covered by 
 * object in given image
 */
class ImageBB {
	const std::vector<glm::vec3> & vertices;
	const bool saveImage = false; //debug feature
		
	struct PointPair {
		glm::ivec2 proj;
		glm::vec3 orig;
	};
		
	/**
	 * >0 - left turn
	 * <0 - right turn
	 * =0 - collinear
	 */
	float orientation(const glm::ivec2& p1, const glm::ivec2& p2, const glm::ivec2& p3) const {
		return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
	}
	
	/**
	 * DEBUG
	 */
	void drawPoint(const glm::ivec2 &p, QImage &img, const QRgb c, const int s = 7) const {
		for(int i = -s; i <= s; ++i) {
			for(int j = -s; j <= s; ++j) {
				img.setPixel(p.x + i, p.y + j, c);
			}
		}
	}
	
public:
	ImageBB(const std::vector<glm::vec3> & vertices) : vertices(vertices) {
	
	}
	
	void computeCameraParams(CameraPosition &cam, const ImageData &img) {
		std::shared_ptr<QImage> qimg;
		if(saveImage) qimg = std::make_shared<QImage>((img.path).c_str());

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
				if(saveImage) drawPoint(pt, *qimg, qRgb(0, 255, 0));
			}
		}
		
		glm::ivec2 c(0, 0);
		
		if(res.size() > 3) { //if there are not enough points, better skip it
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
				while(stack2.size() > 1 && orientation((*(stack2.end()-2)).proj, (*(stack2.end()-1)).proj, p2.proj) >= 0)  {
					stack2.pop_back();
				}
				stack2.push_back(p2);
			}
			res.clear();
			std::reverse(stack2.begin(), stack2.end()); //because area computation
			std::reverse(stack.begin(), stack.end());

			std::copy(stack2.begin(), stack2.end(), std::back_inserter(res));
			std::copy(stack.begin() + 1, stack.end() - 1, std::back_inserter(res));

			
			glm::vec3 centroid(0, 0, 0);
			cam.relativeArea = 0;
		
			for(uint i = 0; i < res.size(); ++i) {
				auto &pt = res[i];
				if(saveImage) drawPoint(pt.proj, *qimg, qRgb(255, 0, 0), 14);
				c += pt.proj;
				centroid += pt.orig;
				if(i + 1 < res.size())
					cam.relativeArea += res[i].proj.x*res[i+1].proj.y - res[i+1].proj.x*res[i].proj.y;
				else
					cam.relativeArea += res[i].proj.x*res[0].proj.y - res[0].proj.x*res[i].proj.y;
			}
			c /= res.size();
			centroid /= res.size();
			cam.relativeArea /= 2.f * img.size.x * img.size.y; //normalize value
			cam.fixedDirection = glm::normalize(centroid - cam.position);
		}
		else {
			//convex hull not found, for whatever reason (possibly detailed image))
			cam.fixedDirection = cam.direction;
			cam.relativeArea = 1.f; /// this is hard to tell, should not happen often though
		}
		
		assert(cam.relativeArea >= 0.f && cam.relativeArea <= 1.f);
		
		if(saveImage) {
			drawPoint(c, *qimg, qRgb(255, 255, 255), 18);
			glm::ivec2 mid = img.size/2;
			drawPoint(mid, *qimg, qRgb(255, 255, 0), 18);
			std::string name = img.path.substr(0, img.path.length() - 4) + "-tmp.jpg";
			qimg->save(name.c_str());
		}
	
	}

};

#endif	/* IMAGEBB_H */

