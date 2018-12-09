#include "actuators.hpp"

void Tempcontrol::calcActions(const TempHumid& ist, const TempHumid& soll)
{
	bool heaterTarget = heat->getStatus();
	bool ventilTarget = false;
	bool tooMoist = ist.humid > soll.humid;
	
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
	else if(heaterTarget)
	{
		fprintf(stderr, "Debounce, keeping Heater ON\n");
	}
	
	if(tooMoist)
	{
		fprintf(stderr, "Too Moist, suggesting Heater ON\n");
		heaterTarget = true;
	}
	
	if(tooMoist && ist.temp > soll.temp + 2)
	{
		fprintf(stderr, "Too Moist and too hot (+2) suggesting Vent ON\n");
		ventilTarget = true;
	}
	
	if(heaterTarget && ist.temp > soll.temp + 3 && ist.temp < 20)	//magic Temperature at which heater just ventilates
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
