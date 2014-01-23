/* 
 * File:   GLWidget.h
 * Author: jaa
 *
 * Created on 22. leden 2014, 18:48
 */

#ifndef GLWIDGET_H
#define	GLWIDGET_H


#include "../BundlerParser.h"
#include "../ShaderHandler.h"
#include "../Camera.h"
#include "../ObjectData.h"
#include "../TextureHandler.h"
#include "../Renderer.h"
#include "../Controlls.h"
#include "../Radar.h"

#include "../RenderPassHandler.h"

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
	
	Radar *radar;
	ObjectData *object;
	Controlls * controlls;
	
	struct timeval start, end;
	int fps;
	
	bool displayRadar;
    
public:
    GLWidget(const QGLFormat& format, int w, int h, QWidget* parent = 0 );
	~GLWidget();
	
	virtual bool eventFilter(QObject *obj, QEvent *event);
	
	void addRenderPass(RenderPass::RenderPassType pass);
	void removeRenderPass(RenderPass::RenderPassType pass);
	
	void setDisplayRadar(bool value);

protected:
    virtual void initializeGL();
    virtual void paintGL();
	
	virtual void resizeGL(int w, int h);
	
};


#endif	/* GLWIDGET_H */

