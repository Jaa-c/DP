
#include <QtCore/qfile.h>
#include <qdatastream.h>

#include "Log.h"
#include "Settings.h"

const QByteArray Settings::fileHeaderByteArray;
quint32	Settings::usingTextures = 4;
glm::vec3 Settings::objectRotate = glm::vec3(0, 0, 0);
	
void Settings::reset() {
	Settings::usingTextures = 4;
	Settings::objectRotate = glm::vec3(0, 0, 0);
}


QDataStream& operator<<(QDataStream& dataStream, const glm::vec3& vec) {
	dataStream << vec.x << vec.y << vec.z;
	return dataStream;
}

QDataStream& operator>>(QDataStream& dataStream, glm::vec3& vec) {
	dataStream >> vec.x >> vec.y >> vec.z;
	return dataStream;
}

void Settings::serialize(std::string name) {
	QFile file(name.c_str());
	if(file.open(QIODevice::WriteOnly)) {
		QDataStream dataStream(&file);
		
		dataStream << usingTextures;
		dataStream << objectRotate;
				
		file.close();
		Log::i("[Settings] Saved configuration file for the scene");
	}
	else {
		throw string("Unable to save configuration file");
	}
};

void Settings::deserialize(std::string name) {
	QFile file(name.c_str());
	if(file.open(QIODevice::ReadOnly)) {
		QDataStream dataStream(&file);

		dataStream >> usingTextures;
		dataStream >> objectRotate;
		
		file.close();
		Log::i("[Settings] Loaded configuration file for the scene");
	}
	else {
		//Log.e("Unable to load configuration file");
	}

};
