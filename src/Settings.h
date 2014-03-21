/* 
 * File:   Settings.h
 * Author: jaa
 *
 * Created on 20. březen 2014, 21:44
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <glm/glm.hpp>
#include <string>


class Settings {
	
	Settings() {}
	
    static const QByteArray fileHeaderByteArray;

public:
	static quint32 usingTextures;
	static glm::vec3 objectRotate;
	
	static void serialize(std::string file);
	static void deserialize(std::string file);
	static void reset();

};

#endif	/* SETTINGS_H */
