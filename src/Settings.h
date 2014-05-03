/* 
 * File:   Settings.h
 * Author: jaa
 *
 * Created on 20. březen 2014, 21:44
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <Qt/qglobal.h>
#include <glm/glm.hpp>
#include <string>


class Settings {
	
	Settings() {}
	
public:
	/// limit for textures bound to fragment shader
	static uint maxTextures;
	/// number of textures currently being used for texturing
	static quint32 usingTextures;
	/// object default rotation
	static glm::vec3 objectRotate;
	/// radar position on screen
	static glm::vec2 radarPos;
	/// radar width and height
	static glm::vec2 radarSize;
	// use directions recomputed in ImageBB
	static bool useRecomputedDirections;
	
	///DEBUG
	static bool usePrefferedCamera;
	static uint prefferedCamera;
	
	static void serialize(std::string file);
	static void deserialize(std::string file);
	static void reset();

};

#endif	/* SETTINGS_H */
