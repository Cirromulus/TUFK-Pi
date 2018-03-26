#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "dht22.hpp"
#include "actuators.hpp"
#include "tinyxml2/tinyxml2.h"

static int DHTPIN = 2;
static int REDPIN = 10;
static int WHTPIN = 11;
static int VNTPIN = 12;
static int FRGPIN = 13;
static int ALMPIN = 14;
static int HETPIN = 6;

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

	
	Led white(WHTPIN);
	Led red(REDPIN);
	Relaisswitch vent(VNTPIN);
	Heater heat(HETPIN);
	
	
	pinMode(FRGPIN, OUTPUT);
	digitalWrite(FRGPIN, HIGH);
	pinMode(ALMPIN, OUTPUT);
	digitalWrite(ALMPIN, HIGH);
	
	Tempcontrol tempcontrol(&vent, &heat);

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
			red.actuate(true);
			delay(500);
			red.actuate(false);
			delay(500);
			delayed += 1000;
		}
		white.actuate(true);
	}
	
	return 0 ;
}
