#include "tinyxml2/tinyxml2.h"

#include <sys/stat.h>

using namespace tinyxml2;

bool fileExists(const char* name)
{
	struct stat buffer;   
	return (stat (name, &buffer) == 0); 
};

void generateDefaultValues(const char* name)
{
	XMLDocument doc;
	XMLDeclaration * decl = doc.NewDeclaration();
	doc.LinkEndChild( decl );
	XMLElement * element = doc.NewElement( "Config" );
	element->SetAttribute("targetTemperature", 16.);
	element->SetAttribute("targetHumidity", 65.);
	doc.LinkEndChild( element );
	doc.SaveFile( name );
};

TempHumid getConfig(XMLDocument& doc)
{
	const XMLElement* elem = doc.FirstChildElement("Config");
	return TempHumid(elem->FindAttribute("targetTemperature")->FloatValue(), 
				     elem->FindAttribute("targetHumidity")->FloatValue());
};

TempHumid loadXmlConfig(const char* filename)
{
	XMLDocument config;
	if(config.LoadFile(filename) != XML_SUCCESS)
	{
		fprintf(stderr, "Could not read config file %s (%s)\n", filename, config.ErrorName());
		
		exit(EXIT_FAILURE);
	}
	return getConfig(config);
}