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
	element->SetAttribute("targetHumidity", 65.);
	element->SetAttribute("samplingPeriodSeconds", 5);
	element->SetAttribute("serverConnectionPeriodSeconds", 60);
	element->SetAttribute("serverURI", "http://wiewarmistesbei.exsilencio.de/");
	doc.LinkEndChild( element );
	doc.SaveFile( name.c_str() );
};

bool Config::reloadFromFile()
{
	if(config.LoadFile(filename.c_str()) != XML_SUCCESS)
	{
		cerr << "Could not read config file " << filename << " " << config.ErrorName() << endl;
		return false;
	}
	const XMLElement* elem = config.FirstChildElement("Config");
	if(elem == nullptr)
		return false;
	if(elem->FindAttribute("targetTemperature") == nullptr)
		return false;
	if(elem->FindAttribute("targetHumidity") == nullptr)
		return false;
	if(elem->FindAttribute("samplingPeriodSeconds") == nullptr)
		return false;
	if(elem->FindAttribute("serverConnectionPeriodSeconds") == nullptr)
		return false;
	if(elem->FindAttribute("serverURI") == nullptr)
		return false;
	return true;
}

bool Config::reloadFromString(std::string xml)
{
	tinyxml2::XMLDocument doc;
	if(doc.Parse(xml.c_str(), xml.length()) != XML_SUCCESS)
	{
		cerr << "Could not read config " << xml << " " << doc.ErrorName() << endl;
		return false;
	}
	const XMLElement* elem = doc.FirstChildElement("Config");
	if(elem == nullptr)
		return false;
	if(elem->FindAttribute("targetTemperature") == nullptr)
		return false;
	if(elem->FindAttribute("targetHumidity") == nullptr)
		return false;
	if(elem->FindAttribute("samplingPeriodSeconds") == nullptr)
		return false;
	if(elem->FindAttribute("serverConnectionPeriodSeconds") == nullptr)
		return false;
	if(elem->FindAttribute("serverURI") == nullptr)
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
