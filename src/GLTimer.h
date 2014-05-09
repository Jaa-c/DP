/* 
 * File:   GLTimer.h
 * Author: jaa
 *
 * Created on 9. květen 2014, 4:21
 */

#ifndef GLTIMER_H
#define	GLTIMER_H

class GLTimer {
	GLuint queryID[2];
	uint queryBack, queryFront;
		
	void swapQueries() {
		if(queryBack) {
			queryBack = 0;
			queryFront = 1;
		}
		else {
			queryBack = 1;
			queryFront = 0;
		}
	}
	
public:
	GLTimer() {
		queryBack = 0;
		queryFront = 1;
		glGenQueries(2, &queryID[0]);
		// dummy query to prevent OpenGL errors from popping out
		glBeginQuery(GL_TIME_ELAPSED, queryID[queryFront]);
	}
	
	void start() {
		glBeginQuery(GL_TIME_ELAPSED, queryID[queryBack]);
	}
	
	///returns elapsed time in ms.
	float end() {
		glEndQuery(GL_TIME_ELAPSED);
		
		GLuint64 time;
		glGetQueryObjectui64v(queryID[queryFront], GL_QUERY_RESULT, &time);
		swapQueries();
		return time / 1000000.0f;
	}

};


#endif	/* GLTIMER_H */
