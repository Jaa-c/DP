/**
 * @file
 * File:   MainWindow.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 23. leden 2014, 16:29
 */

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
 #include <QStatusBar>
 #include <QLabel>

#include <memory>

class GLWidget;
/**
 * Main window, handles GUI stuff only.
 * @return 
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
	friend class GLWidget;
public:
	MainWindow(QApplication *app, int w, int h);
	
	/// Sets FPS to bottom bar
	void setFps(float fps);
	/// Sets number of photos to bottom bar
	void setPhotos(int p);
	
private slots:
	void openCB(); //!< Callback for open button
	void quitCB(); //!< Callback for quit button
	void texturingPassCB(); //!< Callback for texturing render pass
	void texturingPrePassCB(); //!< Callback for texturing pre pass
	void bundlerPointsPassCB(); //!< Callback for camera points button
	void normalsPassCB(); //!< Callback  for normals render pass
	void radarPassCB(); //!< Callback  for display radar button
	void displaySettingsCB(); //!< Callback for opening settings window
	void displayObjectSettingsCB(); //!< Callback  for opening object window
	void reloadShadersCB(); //!< Callback to reload shaders



private:
	void initAppState(); //!< initialize GUI after lauch
	void initScene(); //!< initialize when new scene is opened
	
	void createActions(); //!< creates menu items
	void createMenus(); //!< creates menus
	
	std::shared_ptr<GLWidget> glWidget;
	
	std::shared_ptr<QStatusBar> statusBar;
	
	std::shared_ptr<QLabel> fpsLabel;
	std::shared_ptr<QLabel> fpsValue;
	std::shared_ptr<QLabel> texLabel;
	std::shared_ptr<QLabel> texValue;

	QMenu* file;
	QMenu* renderPass;
	QMenu* view;
	QMenu* settings;
	QMenu* debug;
	
	std::shared_ptr<QAction> openAct;
	std::shared_ptr<QAction> quitAct;
	std::shared_ptr<QAction> texturingRP;
	std::shared_ptr<QAction> texturingPreRP;
	std::shared_ptr<QAction> bundlerPointsRP;
	std::shared_ptr<QAction> radarRP;
	std::shared_ptr<QAction> normalsRP;
	std::shared_ptr<QAction> reloadShaders;
	std::shared_ptr<QAction> programSettings;
	std::shared_ptr<QAction> objectSettings;
	
	std::shared_ptr<QActionGroup> texGroup;
	
	bool init;
};

#endif	/* MAINWINDOW_H */

