#include "actuators.hpp"

void Tempcontrol::calcActions(const TempHumid& ist, const TempHumid& soll)
{
	bool heaterTarget = isHeaterOn;
	bool ventilTarget = isVentOn;
	
	if(tooMoist && ist.humid < soll.humid)
	{
		tooMoist = false;
	}
	
	if(!heat->getStatus() && ist.temp < soll.temp - 1)
	{
		heaterTarget = true;
	}
	else if(heat->getStatus() && !tooMoist && ist.temp > soll.temp + 1)
	{
		heaterTarget = false;
	}
	else if(!heat->getStatus() && ist.temp > soll.temp + 2)
	{
		ventilTarget = true;
	}
	
	if(ist.humid > soll.humid + 10)
	{
		heaterTarget = true;
		tooMoist = true;
	}
	
	//just for safety...
	if(ist.temp > soll.temp + 3)
	{
		heaterTarget = false;
	}
	
	if(heaterTarget != isHeaterOn)
	{
		heat->actuate(heaterTarget);
	}
	if(ventilTarget != isVentOn)
	{
		vent->actuate(ventilTarget);
	}
}
