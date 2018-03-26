/*
 *      dht22.c:
 *	Simple test program to test the wiringPi functions
 *	Based on the existing dht11.c
 *	Amended by technion@lolware.net
 */

#pragma once

#include <wiringPi.h>
#define MAXTIMINGS 85

struct TempHumid
{
	float temp;
	float humid;
};

int read_dht22_dat(TempHumid& out, int pin);