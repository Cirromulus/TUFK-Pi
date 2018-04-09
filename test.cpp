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


int main (int argc, char *argv[])
{
	if (wiringPiSetup () == -1)
	{
		exit(EXIT_FAILURE);
	}

	NoResetActuator beeper(BEEPIN);
	Led white(WHTPIN);
	Led red(REDPIN);
	Led green(GRNPIN);
	Relaisswitch vent(VNTPIN);
	Heater heat(HETPIN, &red);
	Relaisswitch fridge(FRGPIN);
	Relaisswitch alarm(ALMPIN);

	SmokeDetector smokeDetector(SMOKEP);
	Sensor pir(PIRPIN);
	movementSensor = &pir;
	movementLed = &green;
	return 0 ;
}
