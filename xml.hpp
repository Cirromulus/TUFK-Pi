#pragma once

#include "tinyxml2/tinyxml2.h"
#include "temptypes.hpp"

#include <iostream>

enum Override
{
    none = 0b00,
    off  = 0b10,
    on   = 0b11
};

class Config
{
	tinyxml2::XMLDocument config;
	std::string filename;
	bool isComplete(const tinyxml2::XMLDocument& document);
public:
    enum Statusposition
    {
        ventilator = 0,
        heater,
        movement
        fire,
    };

	Config(std::string filename);
	static bool fileExists(std::string name);
	static void generateDefaultValues(std::string name);
	bool reloadFromFile();
	bool reloadFromString(std::string xml);
	TempHumid getTempHumid() const;
	float getTempMaxDelta() const;
	float getTempLowerLimit() const;
	float getTempUpperLimit() const;
	float getHumidLowerLimit() const;
	float getHumidUpperLimit() const;
	uint32_t getSamplingPeriod() const;
	uint32_t getServerConnectPeriod() const;
	uint32_t getActuatorOverrides() const;
	Override getActuatorOverride(uint16_t id) const;
	std::string getServerURI() const;
};
