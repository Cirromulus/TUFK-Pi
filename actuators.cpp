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

void
Heater::actuate(bool newState)
{
	fprintf(stderr, "Toggle heater %s\n", newState ? "ON" : "OFF");
	if(newState && !bootupElapsed)
	{
		time_t now = time(NULL);
		if(now < started + magicStartupTime)
		{
			fprintf(stderr, "Waiting %u seconds for Heater to start up... ", (unsigned)((started + magicStartupTime) - now));
			fflush(stderr);
			blinkDelay(1000 * ((started + magicStartupTime) - now));
			fprintf(stderr, "Done\n");
		}
		bootupElapsed = true;
	}
	if(newState != active)
	{
		system("irsend SEND_START HEATER ONOFF");
		delay(150);
		system("irsend SEND_STOP HEATER ONOFF");
		if(newState)
		{
			delay(250);	//This activates the actual Heating
			system("irsend SEND_START HEATER UP");
			delay(150);
			system("irsend SEND_STOP HEATER UP");
			delay(250);
			//system("irsend SEND_START HEATER DOWN");
			//delay(150);
			//system("irsend SEND_STOP HEATER DOWN");
			//delay(250);
			system("irsend SEND_START HEATER SWING");
			delay(150);
			system("irsend SEND_STOP HEATER SWING");
			
			// this would disable the heating again... 
			//delay(250);
			//system("irsend SEND_ONCE HEATER MODE");
			//delay(250);
			//system("irsend SEND_ONCE HEATER MODE");
		}
		active = newState;
		if(statusLed != nullptr)
		{
			statusLed->actuate(newState);
		}
	}
}
