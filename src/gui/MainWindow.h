/* 
 * File:   MainWindow.h
 * Author: jaa
 *
 * Created on 23. leden 2014, 16:29
 */

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <QMainWindow>

class GLWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QApplication *app, int w, int h);
	~MainWindow();
	
	virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void openCB();
	void quitCB();
	void texturingPassCB();
	void bundlerPointsPassCB();
	void displayRadarCB();



private:
	void initAppState();
	
	void createActions();
	void createMenus();
	
	GLWidget *glWidget;

	QMenu *file;
	QMenu *renderPass;
	QMenu *view;
	
	QAction *openAct;
	QAction *quitAct;
	QAction *texturingRP;
	QAction *bundlerPointsRP;
	QAction *displayRadar;

};

#endif	/* MAINWINDOW_H */

