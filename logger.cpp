/*
 *      dht22.c:
 *	Simple test program to test the wiringPi functions
 *	Based on the existing dht11.c
 *	Amended by technion@lolware.net
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define MAXTIMINGS 85
static int DHTPIN = 2;
static int dht22_dat[5] = {0,0,0,0,0};
FILE* outputfile;

void logFet(float temp, float humid)
{
   static float tempbf, humbf;
   if(temp != tempbf || humid != humbf)
   {
      fprintf(outputfile, "%u,%.2f,%.2f\n", (unsigned)time(NULL), temp, humid);
      fflush(outputfile);
      tempbf = temp;
      humbf = humid;
   }
}

static uint8_t sizecvt(const int read)
{
  /* digitalRead() and friends from wiringpi are defined as returning a value
  < 256. However, they are returned as int() types. This is a safety function */

  if (read > 255 || read < 0)
  {
    printf("Invalid data from wiringPi library\n");
    exit(EXIT_FAILURE);
  }
  return (uint8_t)read;
}

static int read_dht22_dat()
{
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;

  memset(dht22_dat, 0, 5 * sizeof(int));

  // pull pin down for 18 milliseconds
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, HIGH);
  delay(10);
  digitalWrite(DHTPIN, LOW);
  delay(18);
  // then pull it up for 40 microseconds
  digitalWrite(DHTPIN, HIGH);
  delayMicroseconds(40); 
  // prepare to read the pin
  pinMode(DHTPIN, INPUT);

  // detect change and read data
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (sizecvt(digitalRead(DHTPIN)) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) {
        break;
      }
    }
    laststate = sizecvt(digitalRead(DHTPIN));

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      dht22_dat[j/8] <<= 1;
      if (counter > 16)
        dht22_dat[j/8] |= 1;
      j++;
    }
  }

  // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
  // print it out if data is good
  if ((j >= 40) && 
      (dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF)) )
  {
    float t, h;
    h = (float)dht22_dat[0] * 256 + (float)dht22_dat[1];
    h /= 10;
    t = (float)(dht22_dat[2] & 0x7F)* 256 + (float)dht22_dat[3];
    t /= 10.0;
    if ((dht22_dat[2] & 0x80) != 0)
    {
      t *= -1;
    }
    logFet(t, h);
    return 1;
  }
  else
  {
    fprintf(stderr, "Data not good, skip\n");
    return 0;
  }
}

int main (int argc, char *argv[])
{
  int tries = 100;
  int msdelay = 1000;

  if (argc < 2)
  {
	  printf ("usage: %s (<tries> <outputfile> <delay in seconds>)\ndescription: pin is the wiringPi pin number\nusing %d \nOptional: tries is the number of times to try to obtain a read (default 100)",argv[0], DHTPIN);
  }

  if (argc >= 2)
    tries = atoi(argv[1]);

  if(argc >= 3)
  {
    char filename[strlen(argv[2])+20];
    sprintf(filename, "%s%u.csv", argv[2], (unsigned)time(NULL));
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

  if (tries < 0)
  {
    fprintf(stderr, "Endless mode active.\n");
  }

  if(argc >= 4)
  {
    msdelay = atoi(argv[3])*1000;
    fprintf(stdout, "Sensing once every %d seconds.\n", msdelay / 1000);
  }

  if (wiringPiSetup () == -1)
  {
	  exit(EXIT_FAILURE);
  }

	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	fprintf(outputfile, "\"Unix_Timestamp\",\"Temperature_in_°C\",\"Humidity_in_Percent\"\n");
  
  while (read_dht22_dat() == 0 || tries < 0) 
  {
     if(tries >= 0)
     {
          if(tries-- == 0)
	  {
              break;
	  }
     }
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

  delay(1500);

  return 0 ;
}
