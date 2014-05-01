/* 
 * File:   MainWindow.h
 * Author: jaa
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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QApplication *app, int w, int h);
	
	void setFps(float fps);
	void setPhotos(int p);
	
private slots:
	void openCB();
	void quitCB();
	void texturingPassCB();
	void basicTexturingPassCB();
	void bundlerPointsPassCB();
	void normalsPassCB();
	void radarPassCB();
	void displaySettingsCB();
	void displayObjectSettingsCB();
	void useKDTCB();
	
	void reloadShadersCB();



private:
	void initAppState();
	void initScene();
	
	void createActions();
	void createMenus();
	
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
	std::shared_ptr<QAction> basicTexturingRP;
	std::shared_ptr<QAction> bundlerPointsRP;
	std::shared_ptr<QAction> radarRP;
	std::shared_ptr<QAction> normalsRP;
	std::shared_ptr<QAction> reloadShaders;
	std::shared_ptr<QAction> programSettings;
	std::shared_ptr<QAction> objectSettings;
	std::shared_ptr<QAction> useKDT;
	
	std::shared_ptr<QActionGroup> texGroup;

};

#endif	/* MAINWINDOW_H */

