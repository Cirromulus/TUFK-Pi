#pragma once

#include "tinyxml2/tinyxml2.h"
#include "temptypes.hpp"

#include <iostream>

class Config
{
	tinyxml2::XMLDocument config;
	std::string filename;
public:
	Config(std::string filename);
	static bool fileExists(std::string name);
	static void generateDefaultValues(std::string name);
	bool reloadFromFile();
	TempHumid getTempHumid();
	uint32_t getSamplingPeriod();
	uint32_t getServerConnectPeriod();
	std::string getServerURI();
};
