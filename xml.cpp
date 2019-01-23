#include "xml.hpp"

#include <sys/stat.h>

using namespace tinyxml2;
using namespace std;

Config::Config(string filename) : filename(filename)
{
	if(!fileExists(filename))
	{
		std::cout << "No config file found. Creating new one..." << std::endl;
		generateDefaultValues(filename);
	}
	if(!reloadFromFile())
	{
		std::cerr << "Could not open configuration file or invalid." << std::endl;
		exit(EXIT_FAILURE);
	}
}

bool Config::fileExists(std::string name)
{
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
};

void Config::generateDefaultValues(string name)
{
	XMLDocument doc;
	XMLDeclaration * decl = doc.NewDeclaration();
	doc.LinkEndChild( decl );
	XMLElement * element = doc.NewElement( "Config" );
	element->SetAttribute("targetTemperature", 16.);
	element->SetAttribute("temp_upper_limit", 1.);
	element->SetAttribute("temp_lower_limit", 1.);
	element->SetAttribute("targetHumidity", 65.);
	element->SetAttribute("humid_upper_limit", 2.5);
	element->SetAttribute("humid_lower_limit", 2.5);
	element->SetAttribute("samplingPeriodSeconds", 5);
	element->SetAttribute("serverConnectionPeriodSeconds", 60);
	element->SetAttribute("serverURI", "http://wiewarmistesbei.exsilencio.de/");
	doc.LinkEndChild( element );
	doc.SaveFile( name.c_str() );
};

bool isComplete(const tinyxml2::XMLDocument& document)
{
	const XMLElement* elem = document.FirstChildElement("Config");
	if(elem == nullptr)
		return false;
	if(elem->FindAttribute("targetTemperature") == nullptr)
		return false;
	if(elem->FindAttribute("temp_lower_limit") == nullptr)
		return false;
	if(elem->FindAttribute("temp_upper_limit") == nullptr)
		return false;
	if(elem->FindAttribute("targetHumidity") == nullptr)
		return false;
	if(elem->FindAttribute("humid_lower_limit") == nullptr)
		return false;
	if(elem->FindAttribute("humid_upper_limit") == nullptr)
		return false;
	if(elem->FindAttribute("samplingPeriodSeconds") == nullptr)
		return false;
	if(elem->FindAttribute("serverConnectionPeriodSeconds") == nullptr)
		return false;
	if(elem->FindAttribute("serverURI") == nullptr)
		return false;
	return true;
}

bool Config::reloadFromFile()
{
	if(config.LoadFile(filename.c_str()) != XML_SUCCESS)
	{
		cerr << "Could not read config file " << filename << " " << config.ErrorName() << endl;
		return false;
	}
	return isComplete(config);
}

bool Config::reloadFromString(std::string xml)
{
	tinyxml2::XMLDocument doc;
	if(doc.Parse(xml.c_str(), xml.length()) != XML_SUCCESS)
	{
		cerr << "Could not read config " << xml << " " << doc.ErrorName() << endl;
		return false;
	}
	if(!isComplete(doc))
		return false;
	config.Parse(xml.c_str(), xml.length());
	return true;
}

TempHumid Config::getTempHumid()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return TempHumid(elem->FindAttribute("targetTemperature")->FloatValue(),
				     elem->FindAttribute("targetHumidity")->FloatValue());
};

float Config::getTempLowerLimit()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("temp_lower_limit")->FloatValue();
}

float Config::getTempUpperLimit()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("temp_upper_limit")->FloatValue();
}

float Config::getHumidLowerLimit()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("humid_lower_limit")->FloatValue();
}

float Config::getHumidUpperLimit()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("humid_upper_limit")->FloatValue();
}

uint32_t Config::getSamplingPeriod()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("samplingPeriodSeconds")->IntValue();
}

uint32_t Config::getServerConnectPeriod()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("serverConnectionPeriodSeconds")->IntValue();
}

string Config::getServerURI()
{
	const XMLElement* elem = config.FirstChildElement("Config");
	return elem->FindAttribute("serverURI")->Value();
}
