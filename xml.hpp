#pragma once

#include "tinyxml2/tinyxml2.h"
#include "temptypes.hpp"

#include <iostream>

class Config
{
	tinyxml2::XMLDocument config;
	std::string filename;
	bool isComplete(const tinyxml2::XMLDocument& document);
public:
	Config(std::string filename);
	static bool fileExists(std::string name);
	static void generateDefaultValues(std::string name);
	bool reloadFromFile();
	bool reloadFromString(std::string xml);
	TempHumid getTempHumid();
	float getTempLowerLimit();
	float getTempUpperLimit();
	float getHumidLowerLimit();
	float getHumidUpperLimit();
	uint32_t getSamplingPeriod();
	uint32_t getServerConnectPeriod();
	std::string getServerURI();
};
