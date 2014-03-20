
#include "Settings.h"

int Settings::usingTextures = 4;
glm::vec3 Settings::objectRotate = glm::vec3(0, 0, 0);
	
void Settings::reset() {
	Settings::usingTextures = 4;
	Settings::objectRotate = glm::vec3(0, 0, 0);
}

void Settings::serialize(std::string file) {

};

void Settings::deserialize(std::string file) {


};
