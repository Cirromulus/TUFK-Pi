#include "actuators.hpp"


void Tempcontrol::calcActions(const TempHumid& ist, const Config& config)
{
	TempHumid soll = config.getTempHumid();

	bool heaterTarget = false;
	bool ventilTarget = false;
	bool tooMoist = false;


	if(!debounceTooHot && ist.temp < soll.temp - config.getTempLowerLimit())
	{
		fprintf(stderr, "Too cold (-%f), suggesting heater ON\n", config.getTempLowerLimit());
		heaterTarget = true;
		debounceTooHot = true;
	}
	else if(debounceTooHot && ist.temp > soll.temp + config.getTempUpperLimit())
	{
		fprintf(stderr, "Too hot (+%f), suggesting Heater OFF\n", config.getTempUpperLimit());
		heaterTarget = false;
		debounceTooHot = false;
	}
	else if(debounceTooHot)
	{
		fprintf(stderr, "Debounce, keeping Heater ON\n");
		heaterTarget = true;
	}

	if(!debounceTooMoist && ist.humid > (soll.humid + config.getHumidUpperLimit()))
	{
		fprintf(stderr, "Too humid (+%f)\n", config.getHumidUpperLimit());
		tooMoist = true;
		debounceTooMoist = true;
	}
	else if(debounceTooMoist && ist.humid < (soll.humid - config.getHumidLowerLimit()))
	{
		fprintf(stderr, "Dry enough (-%f)\n", config.getHumidLowerLimit());
		tooMoist = false;
		debounceTooMoist = false;
	}
	else if(debounceTooMoist)
	{
		fprintf(stderr, "Debounce, drying Air further\n");
		tooMoist = true;
	}


	//Handle way of drying air
	if(tooMoist)
	{
		fprintf(stderr, "Too Moist, suggesting Heater ON\n");
		heaterTarget = true;
	}

	if(tooMoist && ist.temp > soll.temp + config.getTempUpperLimit())
	{
		fprintf(stderr, "Too Moist and too hot (+%d) suggesting Vent ON\n", config.getTempUpperLimit());
		ventilTarget = true;
	}

	if(heaterTarget && ist.temp > soll.temp + config.getTempMaxDelta() && ist.temp < 22)	//magic Temperature at which heater just ventilates
	{
		fprintf(stderr, "Too Hot (+%d), ignoring moisture, suggesting heater OFF\n", config.getTempMaxDelta());
		heaterTarget = false;
	}
	//----

	if(config.getActuatorOverride(Config::heater) == Override::none)
	{
    	if(heaterTarget != heat->getStatus())
    	{
    		heat->actuate(heaterTarget);
		}
	}
	else
	{
	    printf("Overriding heater to %d\n", config.getActuatorOverride(Config::heater));
        heat->actuate(config.getActuatorOverride(Config::heater) == Override::on);
	}

	if(config.getActuatorOverride(Config::ventilator) == Override::none )
	{
	    if(ventilTarget != vent->getStatus())
	    {
		    vent->actuate(ventilTarget);
	    }
	}
    else
	{
	    printf("Overriding vent to %d\n", config.getActuatorOverride(Config::ventilator));
	    vent->actuate(config.getActuatorOverride(Config::ventilator) == Override::on);
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
		delay(150);
		system("irsend SEND_START HEATER ONOFF");
		fprintf(stderr, "SEND_START HEATER ONOFF ");
		delay(250);
		system("irsend SEND_STOP HEATER ONOFF");
		if(newState)
		{
			delay(250);	//This activates the actual Heating
			system("irsend SEND_START HEATER UP");
			fprintf(stderr, "SEND_START HEATER UP ");
			delay(250);
			system("irsend SEND_STOP HEATER UP");
			delay(250);
			//system("irsend SEND_START HEATER DOWN");
			//delay(150);
			//system("irsend SEND_STOP HEATER DOWN");
			//delay(250);
			system("irsend SEND_START HEATER SWING");
    			fprintf(stderr, "SEND_START HEATER SWING ");
			delay(150);
			system("irsend SEND_STOP HEATER SWING");

			// this would disable the heating again...
			//delay(250);
			//system("irsend SEND_ONCE HEATER MODE");
			//delay(250);
			//system("irsend SEND_ONCE HEATER MODE");
		}
		fprintf(stderr, "\n");
		active = newState;
		if(statusLed != nullptr)
		{
			statusLed->actuate(newState);
		}
		//delay(250);
	}
}
