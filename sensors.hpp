#pragma once
#include <wiringPi.h>

class Sensor
{
	int pin;
public:
	enum class Sensitivity
	{
		falling = INT_EDGE_FALLING,
		rising  = INT_EDGE_RISING,
		both    = INT_EDGE_BOTH,
	};
	
	inline void
	setup()
	{
		pinMode(pin, INPUT);
	};
	
	Sensor(int inputpin) : pin(inputpin)
	{
		setup();
	};
	
	inline int
	getValue()
	{
		return digitalRead(pin);
	}
	inline int 
	registerInterrupt(Sensitivity sensitivity, void (*function)(void))
	{
		int edgeType;
		switch(sensitivity)
		{
			case Sensitivity::falling:
				edgeType = INT_EDGE_FALLING;
				break;
			case Sensitivity::rising:
				edgeType = INT_EDGE_RISING;
				break;
			case Sensitivity::both:
				edgeType = INT_EDGE_BOTH;
				break;
			default:
				edgeType = -1;
		}
		return wiringPiISR (pin, edgeType, function) ;
	}
};

class SmokeDetector : public Sensor
{
public:
	SmokeDetector(int pin) : Sensor(pin){};
	inline int
	getValue()
	{
		return !Sensor::getValue();
	}
};