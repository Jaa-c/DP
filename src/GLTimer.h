/** @file 
 * File:   GLTimer.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 9. květen 2014, 4:21
 */

#ifndef GLTIMER_H
#define	GLTIMER_H

/**
 * Measures time on GPU using asynchrous queries.
 */
class GLTimer {
	GLuint queryID[2];
	uint queryBack, queryFront;
	
	void swapQueries() {
		if(queryBack) {
			queryBack = 0, queryFront = 1;
		}
		else {
			queryBack = 1, queryFront = 0;
		}
	}
	
public:
	GLTimer() {
		queryBack = 0, queryFront = 1;
		glGenQueries(2, &queryID[0]);
		// dummy query to prevent OpenGL errors from popping out
		glBeginQuery(GL_TIME_ELAPSED, queryID[queryFront]);
	}
	/**
	 * Starts timer on back query
     */
	void start() {
		glBeginQuery(GL_TIME_ELAPSED, queryID[queryBack]);
	}
	
	/**
	 * returns time from previous query and swaps queries
	 */
	float end() {
		glEndQuery(GL_TIME_ELAPSED);
		GLuint64 time;
		glGetQueryObjectui64v(queryID[queryFront], GL_QUERY_RESULT, &time);
		swapQueries();
		return time / 1000000.0f;
	}

};


#endif	/* GLTIMER_H */
