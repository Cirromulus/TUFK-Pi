#include "actuators.hpp"

void Tempcontrol::calcActions(const TempHumid& ist, const TempHumid& soll)
{
	bool heaterTarget = heat->getStatus();
	bool ventilTarget = false;
	
	
	if(!heat->getStatus() && ist.temp < soll.temp - 1)
	{
		fprintf(stderr, "Too cold, suggesting heater ON\n");
		heaterTarget = true;
	}
	else if(heat->getStatus() && ist.temp > soll.temp + 1)
	{
		fprintf(stderr, "Too hot (+1), suggesting Heater OFF\n");
		heaterTarget = false;
	}
	
	if(ist.humid > soll.humid + 10)
	{
		fprintf(stderr, "Too Moist, suggesting Heater ON\n");
		heaterTarget = true;
	}
	
	if(ist.temp > soll.temp + 2)
	{
		fprintf(stderr, "Too hot (+2) suggesting Vent ON\n");
		ventilTarget = true;
	}
	
	if(ist.temp > soll.temp + 3)
	{
		fprintf(stderr, "Too Hot (+3), ignoring moisture, suggesting heater OFF\n");
		heaterTarget = false;
	}
	
	if(heaterTarget != heat->getStatus())
	{
		heat->actuate(heaterTarget);
	}
	if(ventilTarget != vent->getStatus())
	{
		vent->actuate(ventilTarget);
	}
}
