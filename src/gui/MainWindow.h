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

class GLWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QApplication *app, int w, int h);
	~MainWindow();
	
private slots:
	void openCB();
	void quitCB();
	void texturingPassCB();
	void basicTexturingPassCB();
	void bundlerPointsPassCB();
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
	
	GLWidget *glWidget;

	QMenu *file;
	QMenu *renderPass;
	QMenu *view;
	QMenu *settings;
	QMenu *debug;
	
	QAction *openAct;
	QAction *quitAct;
	QAction *texturingRP;
	QAction *basicTexturingRP;
	QAction *bundlerPointsRP;
	QAction *radarRP;
	QAction *reloadShaders;
	QAction *programSettings;
	QAction *objectSettings;
	QAction *useKDT;
	
	QActionGroup * texGroup;

};

#endif	/* MAINWINDOW_H */

