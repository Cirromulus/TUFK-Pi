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
	const TempHumid getTempHumid();
	const float getTempLowerLimit();
	const float getTempUpperLimit();
	const float getHumidLowerLimit();
	const float getHumidUpperLimit();
	const uint32_t getSamplingPeriod();
	const uint32_t getServerConnectPeriod();
	const std::string getServerURI();
};
