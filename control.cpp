#include <wiringPi.h>
#include <mcp23017.h>

#include "dht22.hpp"
#include "actuators.hpp"
#include "sensors.hpp"
#include "config.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <functional>


volatile sig_atomic_t done;

void printCsvHeader()
{
	printf("\"Unix_Timestamp\",\"Temperature_in_°C\",\"Humidity_in_Percent\",\"Heater\",\"Vent\"\n");
}

void logCsv(const TempHumid& th, bool isHeaterOn, bool isVentOn)
{
	printf("%u,%.2f,%.2f,%d,%d\n", (unsigned)time(NULL), th.temp, th.humid, isHeaterOn, isVentOn);
	fflush(stdout);
}

void term(int signum)
{
	fprintf(stderr, "Received signal %d\n", signum);
	done = true;
}

Actuator* globalBeeper = nullptr;
Sensor* globalSmoke = nullptr;
void ohNoez()
{
	if(globalSmoke != nullptr)
	{
		//dehysterese
		printf("Smoke: %s\n", globalSmoke->getValue() ? "yes" : "no");
		if(!globalSmoke->getValue())
		{
			return;
		}
	}
	if(globalBeeper != nullptr)
	{
		globalBeeper->actuate(true);
	}
	fprintf(stderr, "Oh NOEZ, es brennt!\n");
	term(0);
	term(118);
	term(999);
	term(881);
	term(999);
	term(119);
	term(725);
	term(3);
}

//Ugly
Sensor* movementSensor = nullptr;
Led* movementLed = nullptr;
void movementChanged()
{
	if(movementSensor != nullptr)
	{
		int val = movementSensor->getValue();
		if(val)
		{
			fprintf(stderr, "Movement detected\n");
		}
		else
		{
			fprintf(stderr, "Movement ended (possibly)\n");
		}
		if(movementLed != nullptr)
		{
			movementLed->actuate(val);
		}
	}
}

int main (int argc, char *argv[])
{
	int msdelay = 2000;
	TempHumid target;
	
	done = false;
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	
	if (argc < 3)
	{
		printf ("usage: %s temperature humidity\n", argv[0]);
		return -EINVAL;
	}

	target = TempHumid(atof(argv[1]), atof(argv[2]));
	
	fprintf(stderr, "Target: %0.2f °C, %0.2f%%\n", target.temp, target.humid);
	
	if (wiringPiSetup () == -1)
	{
		exit(EXIT_FAILURE);
	}
	mcp23017Setup (EXPANDER_BASE_PIN, EXPANDER_ADDR) ;

	NoResetActuator beeper(ALARMPIN);
	globalBeeper = &beeper;
	Led white(WHITELED);
	Led red(REDLED);
	Led green(GREENLED);
	Relaisswitch vent(VENTILATORPIN);
	Heater heat(HEATERPIN, &red);
	Relaisswitch fridge(FRIDGEPIN);
	Relaisswitch pa(PAAMPPIN);

	SmokeDetector smokeDetector(GASSENS);
	globalSmoke = &smokeDetector;
	if(smokeDetector.getValue())
	{
		beeper.actuate(true);
		fprintf(stderr, "Oh NOEZ, es brennt!\n");
		return EXIT_FAILURE;
	}
	smokeDetector.registerInterrupt(Sensor::Sensitivity::both, ohNoez);
	Sensor pir(PIRPIN);
	movementSensor = &pir;
	movementLed = &green;
	pir.registerInterrupt(Sensor::Sensitivity::both, movementChanged);


	Tempcontrol tempcontrol(&heat, &vent);
	printCsvHeader();
	TempHumid curr, last;
	while (!done) 
	{
		int ret = read_dht22_dat(curr, DHT22PIN);
		if(ret < 0)
		{
			continue;
		}
		if(curr != last)
		{
			logCsv(curr, heat.getStatus(), vent.getStatus());
			tempcontrol.calcActions(curr, target);
			last = curr;
		}

		white.actuate(false);
		unsigned delayed = 0;
		while(delayed < msdelay)
		{
			if(done)
			{
				break;
			}
			delay(250);
			delayed += 250;
		}
		white.actuate(true);
	}

	return 0 ;
}
