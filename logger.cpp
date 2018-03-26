#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "dht22.hpp"

static int DHTPIN = 2;
FILE* outputfile;

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


int main (int argc, char *argv[])
{
	int msdelay = 1000;

	if (argc < 2)
	{
		printf ("usage: %s (<outputfile> <delay in seconds>)\ndescription: pin is the wiringPi pin number\nusing %d \nOptional: tries is the number of times to try to obtain a read (default 100)",argv[0], DHTPIN);
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

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	fprintf(outputfile, "\"Unix_Timestamp\",\"Temperature_in_°C\",\"Humidity_in_Percent\"\n");
	TempHumid th;
	while (true) 
	{
		int ret = read_dht22_dat(th, DHTPIN);
		if(ret < 0)
		{
			continue;
		}
		logTH(th);
		digitalWrite(11, LOW);
		unsigned delayed = 0;
		while(delayed < msdelay)
		{
			digitalWrite(10, HIGH);
			delay(500);
			digitalWrite(10, LOW);
			delay(500);
			delayed += 1000;
		}
		digitalWrite(11, HIGH);
	}
	return 0 ;
}
