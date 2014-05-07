/* 
 * File:   GLWidget.h
 * Author: jaa
 *
 * Created on 22. leden 2014, 18:48
 */

#ifndef GLWIDGET_H
#define	GLWIDGET_H


#include "../io/BundlerParser.h"
#include "../ShaderHandler.h"
#include "../Camera.h"
#include "../ObjectData.h"
#include "../io/ImageLoader.h"
#include "../Renderer.h"
#include "../Controlls.h"
#include "../io/CalibrationLoader.h"

#include "../RenderPassHandler.h"
#include "MainWindow.h"

#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>

class RayCaster;

class GLWidget : public QGLWidget
{
	Q_OBJECT
	
	static const std::string settingsExt;
			
	RenderPassHandler renderPassHandler;
	Camera camera;
	ShaderHandler shaderHandler;
	Renderer renderer;
	Controlls &controlls;
	
	std::shared_ptr<TextureHandler> textureHandler;
	std::shared_ptr<ObjectData> object;
	
	struct timeval start, end;
	int fps;
	
	MainWindow& mainWin;
	
	friend class MainWindow;
    
public:
	
    GLWidget(const QGLFormat& format, int w, int h, QWidget* parent = 0 );
	~GLWidget();
	
	void createScene(
		std::string geom, 
		std::string photos, 
		CalibrationLoader::FileType type,
		std::string file1,
		std::string file2 = "");
	void deleteScene();
	
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

