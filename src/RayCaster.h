/* 
 * File:   RayCaster.h
 * Author: jaa
 *
 * Created on 6. duben 2014, 15:25
 */

#ifndef RAYCASTER_H
#define	RAYCASTER_H

#include "ObjectData.h"

struct Cluster {
	glm::vec3 centroid;
	std::vector<const glm::vec3 *> vectors;	
};


class RayCaster {
	const ObjectData &object;
	const Camera &camera;
	
	static const int n = 100;
	static const int clusterCount = 5;
	std::array<Cluster, clusterCount> clusters;

public:
	RayCaster(const ObjectData& o, const Camera &c) :
		object(o), camera(c)
	{
	
	}

	void cast(const glm::vec3 &viewDir) {
		const glm::vec3 view = glm::normalize(viewDir);
		
		glm::ivec2 winSize = camera.getWindowSize();
		float p = std::sqrt(n * winSize.y / (float) winSize.x);
		int dx = std::ceil(winSize.x / (p * winSize.x / (float) winSize.y));
		int dy = std::ceil(winSize.y / p);
				
		glm::mat4 mvp = camera.getProjectionMatrix() * camera.getModelViewMatrix() * object.getMvm();
		glm::mat4 invPM(glm::inverse(mvp));
		
		std::default_random_engine gen;
		std::uniform_int_distribution<int> distr(0, clusterCount-1);
		int intersections = 0;
		for(int i = 0; i < winSize.x; i+=dx) {
			for(int j = 0; j < winSize.y; j+=dy) {
				glm::vec4 o(2.f / winSize.x * i - 1, 2.f / winSize.y * j - 1, -1, 1);
				glm::vec4 d(o.x, o.y, -o.z, o.w);
				
				o = invPM * o;
				d = invPM * d;
				
				glm::vec3 orig(o / o.w);
				glm::vec3 dir(d / d.w);
				dir = glm::normalize(dir - orig);
				
				float tmin = 10e6;
				float t = 0;
				int triangleIdx = -1;
				for(uint n = 0; n < object.getIndices().size(); n += 3) {
					if(glm::dot(object.getNormals()[object.getIndices()[n]], view) > 0) {
						if(getIntersection(n, t, orig, dir) && t < tmin) {
							tmin = t;
							triangleIdx = n;
						}
					}
				}
				if(triangleIdx != -1) {
					const glm::vec3 &norm = object.getNormals()[triangleIdx];
					int d = (int) ((glm::dot(view, norm) + 1) / 2.f * 4 + 0.5);
					d = distr(gen);
					clusters[d].vectors.push_back(&norm);
					intersections++;
				}
			}
		}
		
//		std::cout << "found " << intersections << " intersections\n";
		
		bool moving = true;
		int iterations = 0;
		while(moving && iterations < 20) {
			moving = false;
			
			for(Cluster &c : clusters) {
				if(!c.vectors.empty()) {
					for(auto *v : c.vectors) {
						c.centroid += *v;
					}
					c.centroid /= (float) c.vectors.size();
					c.centroid = glm::normalize(c.centroid);
				}
				else {
					c.centroid *= 0;
				}
			}
		
			float dist = -1;
			for(Cluster &c : clusters) {
				for(auto v = c.vectors.begin(); v != c.vectors.end();) {
					dist = glm::dot(**v, c.centroid);
//					if(c.vectors.size() == 1) {
//						dist = 0;
//					}
					Cluster *moveTo = nullptr;
					for(Cluster &cl: clusters) {
						float diff = glm::dot(**v, cl.centroid);
						if(diff > dist + 0.01) {
							moveTo = &cl;
						}
					}
					if(moveTo) {
						moveTo->vectors.push_back(*v);
						v = c.vectors.erase(v);
						moving = true;
					}
					else {
						++v;
					}
				}
			}
			++iterations;
//			std::cout << "iter: " << iterations << "\n";
//			for(Cluster &cl: clusters) {
//				std::cout << "\t" << cl.vectors.size() << " : " <<
//						cl.centroid[0] << ", " << cl.centroid[1] << ", " << cl.centroid[2] << "\n";
//			
//			}
		}
		
		std::sort(clusters.begin(), clusters.end(), 
				[] (const Cluster &a, const Cluster &b) {
					return a.vectors.size() > b.vectors.size();
				}
		);
//		std::cout << "Clusters found in " << iterations << " iterations\n";
	}
	
	const std::array<Cluster, clusterCount> &getClusters() const {
		return clusters;
	}
	
	
	
private:
	bool getIntersection(const GLuint ind, float &t, const glm::vec3 &orig, const glm::vec3 &dir) {
		const glm::vec3 &a = object.getVertices()[object.getIndices()[ind]];
		const glm::vec3 &b = object.getVertices()[object.getIndices()[ind+1]];
		const glm::vec3 &c = object.getVertices()[object.getIndices()[ind+2]];
		const glm::vec3 e1 = b - a;
		const glm::vec3 e2 = c - a;

		const glm::vec3 s1 = glm::cross(dir, e2);

		float divisor = glm::dot(s1, e1);

		if (divisor == 0)
			return false;

		float invDivisor = 1.0f / divisor;

		// Compute first barycentric coordinate
		const glm::vec3 d = orig - a;

		float b1 = glm::dot(d, s1) * invDivisor;

		if (b1 < 0. || b1 > 1.)
			return false;

		// Compute second barycentric coordinate
		const glm::vec3 s2 = glm::cross(d, e1);

		const float b2 = glm::dot(dir, s2) * invDivisor;

		if (b2 < 0. || b1 + b2 > 1.)
			return false;

		// Compute t to intersection point
		t = glm::dot(e2, s2) * invDivisor;

		// TODO
		/*if (t < ray.mint || t > ray.maxt)
			return false;*/

		return true;
	}



};


#endif	/* RAYCASTER_H */

