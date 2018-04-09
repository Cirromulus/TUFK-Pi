//TODO: heater needs time and a plus signal. get long twowire cable.

#include "dht22.hpp"
#include "actuators.hpp"
#include "sensors.hpp"
#include "tinyxml2/tinyxml2.h"
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

void logTH(const TempHumid& th)
{
   static float tempbf, humbf;
   if(th.temp != tempbf || th.humid != humbf)
   {
      printf("%u,%.2f,%.2f\n", (unsigned)time(NULL), th.temp, th.humid);
      fflush(stdout);
      tempbf = th.temp;
      humbf = th.humid;
   }
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
		//FIXME: This disables the Firealarm.
		return;
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
			fprintf(stderr, "Movement ended (probably)\n");
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

	NoResetActuator beeper(BEEPIN);
	globalBeeper = &beeper;
	Led white(WHTPIN);
	Led red(REDPIN);
	Led green(GRNPIN);
	Relaisswitch vent(VNTPIN);
	Heater heat(HETPIN, &red);
	Relaisswitch fridge(FRGPIN);
	Relaisswitch alarm(ALMPIN);

	SmokeDetector smokeDetector(SMOKEP);
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

	printf("\"Unix_Timestamp\",\"Temperature_in_°C\",\"Humidity_in_Percent\"\n");
	TempHumid th;
	while (!done) 
	{
		int ret = read_dht22_dat(th, DHTPIN);
		if(ret < 0)
		{
			continue;
		}
		logTH(th);
		tempcontrol.calcActions(th, target);	
		
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
