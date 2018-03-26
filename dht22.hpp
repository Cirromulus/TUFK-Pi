/*
 *      dht22.c:
 *	Simple test program to test the wiringPi functions
 *	Based on the existing dht11.c
 *	Amended by technion@lolware.net
 */

#pragma once

#include <wiringPi.h>
#include "temptypes.hpp"
#define MAXTIMINGS 85

int read_dht22_dat(TempHumid& out, int pin);