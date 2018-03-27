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

static int DHTPIN = 2;
FILE* outputfile;

volatile sig_atomic_t done;

void logTH(TempHumid& th)
{
   static float tempbf, humbf;
   if(th.temp != tempbf || th.humid != humbf)
   {
      fprintf(outputfile, "%u,%.2f,%.2f\n", (unsigned)time(NULL), th.temp, th.humid);
      fflush(outputfile);
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
	int msdelay = 1000;
	done = false;
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	
	if (argc < 2)
	{
		printf ("usage: %s (<outputfile> <delay in seconds>)\n",argv[0], DHTPIN);
	}

	if(argc >= 2)
	{
		char filename[strlen(argv[1])+20];
		sprintf(filename, "%s%u.csv", argv[1], (unsigned)time(NULL));
		outputfile = fopen(filename, "a");
		if(outputfile < 0)
		{
			fprintf(stderr, "Could not open file.\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stdout, "Using file %s as output.\n", filename);
	}
	else
	{
		outputfile = stdout;
	}

	if(argc >= 3)
	{
		msdelay = atoi(argv[2])*1000;
		fprintf(stdout, "Sensing once every %d seconds.\n", msdelay / 1000);
	}

	if (wiringPiSetup () == -1)
	{
		exit(EXIT_FAILURE);
	}

	Led white(11);
	Led red(10);

	fprintf(outputfile, "\"Unix_Timestamp\",\"Temperature_in_°C\",\"Humidity_in_Percent\"\n");
	TempHumid th;
	while (!done) 
	{
		int ret = read_dht22_dat(th, DHTPIN);
		if(ret < 0)
		{
			continue;
		}
		logTH(th);
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
