/* 
 * File:   GLWidget.h
 * Author: jaa
 *
 * Created on 22. leden 2014, 18:48
 */

#ifndef GLWIDGET_H
#define	GLWIDGET_H


#include "BundlerParser.h"
#include "ShaderHandler.h"
#include "Camera.h"
#include "ObjectData.h"
#include "TextureHandler.h"
#include "Renderer.h"
#include "Controlls.h"

#include "RenderPassHandler.h"


#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>

class GLWidget : public QGLWidget
{
	Q_OBJECT
	
	BundlerParser bp;
	RenderPassHandler renderPassHandler;
	Camera camera;
	ShaderHandler shaderHandler;
	Renderer renderer;
	TextureHandler textureHandler;
	
	ObjectData *object;
	Controlls * controlls;
	
	struct timeval start, end;
	int fps;
    
public:
    GLWidget(const QGLFormat& format, int w, int h, QWidget* parent = 0 );
	~GLWidget();
	
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected:
    virtual void initializeGL();
    virtual void paintGL();
	
	virtual void resizeGL(int w, int h);
	
	
	inline void drawRadar(GLint x, GLint y, GLsizei width, GLsizei height)
	{
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

		glm::vec4 tmp, p1, p2, dir;
		glm::vec3 c, k;
		c = object->getCentroidPosition();
		k = -camera.getCameraPosition();

		const Points &cameras = object->pointData->getCameraPositions();
		const Vectors &cameraDirections = bp.getCamerDirections();
		const glm::mat4 vecMat = glm::inverse(glm::transpose(object->mvm));

		glBegin(GL_LINES);
			glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
			for(uint i = 0; i < cameraDirections.size(); ++i) {
				p1 =  object->mvm * glm::vec4(cameras.at(i), 1.0f);
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
			for(auto it = cameras.begin(); it != cameras.end(); ++it) {
				tmp = object->mvm * glm::vec4((*it), 1.0f);
				drawPoint(tmp);
			}
		glEnd();

		glPointSize(5.0);
		glBegin(GL_POINTS);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			drawPoint(k);

			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			drawPoint(c);

			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			const int camID = controlls->getCameraId();
			tmp =  object->mvm * glm::vec4(cameras[camID], 1.0f);
			drawPoint(tmp);
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

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
		d *= 3;
		glm::vec2 p1(point.x, point.z);
		glm::vec2 p2 = p1 + d;
		if(cmpCoords(p1) && cmpCoords(p2)) {
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
		}	
	}

	bool cmpCoords(glm::vec2 &p) {
		const glm::vec2 xr(-22.f, 18.f);
		const glm::vec2 yr(5.f, 45.f);

		p.x = (p.x - xr.x) * (260.f - 10.f) / (xr.y - xr.x) + 10.f;
		p.y = (p.y - yr.x) * (260.f - 10.f) / (yr.y - yr.x) + 260.f;

		if(p.x < 10 || p.x > 260 || p.y < 10 || p.y > 260) {
			return false;
		}
		return true;
	}
};


#endif	/* GLWIDGET_H */

