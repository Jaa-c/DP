/* 
 * File:   Log.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 23:20
 */

#ifndef LOG_H
#define	LOG_H

#include <iostream>
#include <memory>
#include <cstdarg>
#include <cstring>

using std::cout;
using std::string;

class Log {
		
	enum Level {
		LOG_INFO,
		LOG_DEBUG,
		LOG_ERROR
	};
	
	static void log(const string msg, const Level lvl) {
		switch(lvl) {
			case LOG_INFO:
				cout << "INFO:  ";
				break;
			case LOG_DEBUG:
				cout << "DEBUG: ";
				break;
			case LOG_ERROR:
				cout << "ERROR: ";
				break;
		}
		cout << msg << "\n";
	}
		
	/// taken from: http://stackoverflow.com/a/8098080/683905
	static string string_format(const string fmt_str, va_list args) {
		int n = fmt_str.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
		std::unique_ptr<char[]> formatted;
		while(1) {
			formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
			strcpy(&formatted[0], fmt_str.c_str());
			int final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), args);
			if (final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}
		return string(formatted.get());
	}

public:
	static void i(const string msg, ...) {
		va_list argptr;
		va_start(argptr, msg);
		log(string_format(msg, argptr), LOG_INFO);
		va_end(argptr);
	}
	
	static void d(const string msg, ...) {
		va_list argptr;
		va_start(argptr, msg);
		log(string_format(msg, argptr), LOG_DEBUG);
		va_end(argptr);
	}
	
	static void e(const string msg, ...) {
		va_list argptr;
		va_start(argptr, msg);
		log(string_format(msg, argptr), LOG_ERROR);
		va_end(argptr);
	}

};

#endif	/* LOG_H */
