/* 
 * File:   GLWidget.h
 * Author: jaa
 *
 * Created on 22. leden 2014, 18:48
 */

#ifndef GLWIDGET_H
#define	GLWIDGET_H


#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>

class Main;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget(Main * main, const QGLFormat& format, QWidget* parent = 0 );
	~GLWidget() {}

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();
private:
	Main * main;
};


#endif	/* GLWIDGET_H */

