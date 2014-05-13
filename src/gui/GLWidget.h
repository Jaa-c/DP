/* *
 * @file
 * File:   GLWidget.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
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
#include "../GLTimer.h"

#include "../RenderPassHandler.h"
#include "MainWindow.h"

#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>


/**
 * Widget that draws to screen via OpenGL.
 * Handles main loop and scene initialization. 
 */
class GLWidget : public QGLWidget
{
	Q_OBJECT
	
	static const std::string settingsExt; //!< suffix for the settings file
			
	RenderPassHandler renderPassHandler; //!< needed for draw calls
	Camera camera; //!< OpenGL camera
	ShaderHandler shaderHandler; //!< instance passed to render passes
	Renderer renderer; //!< draws stuff
	Controlls &controlls; //!< Handles inputs
	
	std::shared_ptr<TextureHandler> textureHandler; //!< Handles texture loading
	std::shared_ptr<ObjectData> object; //!< the object that is drawn
	
	std::shared_ptr<GLTimer> gltimer;  //!< FPS measurments
	int f; //!< used for fps display purposes
	
	MainWindow& mainWin;  //!< reference to parent
	
	friend class MainWindow;
    
public:
	/**
	 * 
     * @param format OpenGL format
     * @param w initial window width
     * @param h initial window height
     * @param parent parent widget
     */
    GLWidget(const QGLFormat& format, int w, int h, QWidget* parent = 0 );
	~GLWidget();
	
	/**
	 * Creates new scene when object is openede
     * @param geom path to file with the geometry
     * @param photos path to folder with fotos
     * @param type type of the calibration file
     * @param file1 calibration data file 1
     * @param file2 calibration data file 2 if needed
     */
	void createScene(
		std::string geom, 
		std::string photos, 
		CalibrationLoader::FileType type,
		std::string file1,
		std::string file2 = "");
	/// deletes scene stuff if new scene is opened
	void deleteScene();
	
	///Qt event filter that handles and delegates inputs
	virtual bool eventFilter(QObject *obj, QEvent *event);
	
	/**
	 * Add render pass to rendering pipeline
     * @param pass which pass to add
     */
	void addRenderPass(RenderPass::RenderPassType pass);
	/**
	 * Removes pass from rendering, if used
     * @param pass pass to remove
     */
	void removeRenderPass(RenderPass::RenderPassType pass);
	

protected:
	/// Initializes OpenGL state
    virtual void initializeGL();
	/// Main loop
    virtual void paintGL();
	/// Window resize callback
	virtual void resizeGL(int w, int h);
	
};


#endif	/* GLWIDGET_H */

