/* 
 * File:   RadarRenderPass.h
 * Author: jaa
 *
 * Created on 21. březen 2014, 16:20
 */

#ifndef RADARRENDERPASS_H
#define	RADARRENDERPASS_H

#include "../Settings.h"
#include "RenderPass.h"

#include <GL/glew.h>

class RadarRenderPass : public RenderPass {
	
	Camera &camera;
	
	
	//coordinates mapped on radar
	glm::vec2 xr, yr;
	int x, y, width, height;
	
public:
	
	RadarRenderPass(Renderer *r, ShaderHandler *sh, TextureHandler *th, Camera &camera) : 
		RenderPass(RADAR_PASS, r, sh, th), camera(camera) {}
	
	
	~RadarRenderPass() {

	}
	
	/**
	 * TODO: refactor if there is time
	 * Please don't look any further.
     */
	void draw(ObjectData *object) {
		if(!object->pointData) {
			return;
		}
		
		const Points &cameras = object->pointData->getCameraPositions();
		const Vectors &cameraDirections = object->pointData->getCameraDirections();
		const glm::mat4 vecMat = glm::inverse(glm::transpose(object->getMvm()));
		
		glm::vec2 xlimits(10e5, -10e5);
		glm::vec2 ylimits(10e5, -10e5);
		glm::vec4 tmp;
		
		for(auto &cam : cameras) {
			tmp = object->getMvm() * glm::vec4(cam, 1.0f);
			if(tmp.x < xlimits.s) xlimits.s = tmp.x;
			if(tmp.x > xlimits.t) xlimits.t = tmp.x;
			if(tmp.z < ylimits.s) ylimits.s = tmp.z;
			if(tmp.z > ylimits.t) ylimits.t = tmp.z;
		}
		float diffx = fabs(xlimits.y - xlimits.x);
		float diffy = fabs(ylimits.y - ylimits.x);
		
		float size = diffx > diffy ? diffx : diffy;
		size *= 0.7f;
		
		xr.x = (xlimits.y + xlimits.x) / 2.0f - size;
		xr.y = (xlimits.y + xlimits.x) / 2.0f + size;
		yr.x = (ylimits.y + ylimits.x) / 2.0f - size;
		yr.y = (ylimits.y + ylimits.x) / 2.0f + size;
		
		x = Settings::radarPos.x;
		y = Settings::radarPos.y;
		width = Settings::radarSize.x;
		height = Settings::radarSize.y;
		
		GLint viewport[4] = {0};
		glGetIntegerv(GL_VIEWPORT, viewport);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, viewport[2], 0, viewport[3], -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable( GL_BLEND );

		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glBegin(GL_QUADS);
			glVertex2i(x, y);
			glVertex2i(x+width, y);
			glVertex2i(x+width, y+height);
			glVertex2i(x, y+height);
		glEnd();

		glEnable(GL_POINT);
		glEnable(GL_POINT_SIZE);
		glPointSize(4.0);

		glm::vec4 p1, dir;
		glm::vec3 c, k;
		c = object->getCentroidPosition();
		k = camera.getCameraPosition();

		glBegin(GL_LINES);
			glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
			for(uint i = 0; i < cameraDirections.size(); ++i) {
				p1 = object->getMvm() * glm::vec4(cameras.at(i), 1.0f);
				dir = vecMat * glm::vec4(cameraDirections.at(i), 1.0f);
				drawLine(p1, dir);
			}

			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			glm::vec3 viewDir = -camera.getCameraViewDirection();
			drawLine(k, viewDir);

			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			drawPoint(k);
			drawPoint(c);
		glEnd();

		glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
		glBegin(GL_POINTS);
			for(auto &cam : cameras) {
				tmp = object->getMvm() * glm::vec4(cam, 1.0f);
				drawPoint(tmp);
			}
		glEnd();

		glPointSize(5.0);
		glBegin(GL_POINTS);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			drawPoint(k);

			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			drawPoint(c);
			
			glColor4f(1.0f, 1.0f, 0.0f, 1.0f); //current in RAM
			for(auto p : textureHandler->nearPhotos) {
				tmp = object->getMvm() * glm::vec4(p.second->camera.position, 1.0f);
				drawPoint(tmp);
			}

			glColor4f(0.0f, 0.0f, 1.0f, 1.0f); //current on GPU
			for(auto &tex : textureHandler->getTextures()) {
				tmp = object->getMvm() * glm::vec4(tex.photo->camera.position, 1.0f);
				drawPoint(tmp);
			}
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	
private:

	void drawPoint(glm::vec4 &p) {
		glm::vec2 tmp(p.x, p.z);
		drawPoint(tmp);
	}
	void drawPoint(glm::vec3 &p) {
		glm::vec2 tmp(p.x, p.z);
		drawPoint(tmp);
	}
	void drawPoint(glm::vec2 &p) {
		if(cmpCoords(p)) {
			glVertex2f(p.x, p.y);
		}
	}
	void drawLine(glm::vec4 &point, glm::vec4 &dir) {
		glm::vec3 p(point);
		glm::vec3 d(dir);
		drawLine(p, d);
	}
	void drawLine(glm::vec3 &point, glm::vec3 &dir) {
		glm::vec2 d(dir.x, dir.z);
		d = glm::normalize(d);
		d *= 15;
		glm::vec2 p1(point.x, point.z);
		//glm::vec2 p2 = p1 + d;
		if(cmpCoords(p1)) {
			glVertex2f(p1.x, p1.y);
			glm::vec2 p2 = p1 + d;
			glVertex2f(p2.x, p2.y);
		}	
	}

	bool cmpCoords(glm::vec2 &p) {
		p.x = (p.x - xr.x) * (width) / (xr.y - xr.x) + x;
		p.y = (p.y - yr.x) * (height) / (yr.y - yr.x) + y;

		if(p.x < 10 || p.x > 260 || p.y < 10 || p.y > 260) {
			return false;
		}
		return true;
	}
	
};


#endif	/* RADARRENDERPASS_H */

