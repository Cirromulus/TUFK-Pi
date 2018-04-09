#pragma once

#include "temptypes.hpp"

#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>

class Actuator
{
protected:
	bool active;
public:
	virtual void
	actuate(bool) = 0;
	virtual inline void
	setup()
	{
		actuate(false);
	}
	inline bool getStatus()
	{
		return active;
	}
	inline
	Actuator() : active(false){};
	virtual inline
	~Actuator()
	{};
};

class PinActuator : public Actuator
{
protected:
	int pin;
public:
	inline
	PinActuator(int pin) : pin(pin)
	{
		setup();
	};
	virtual inline
	~PinActuator()
	{
		actuate(false);
	}
	inline void
	setup()
	{
		pinMode(pin, OUTPUT);
		Actuator::setup();
	}
	inline void
	actuate(bool status)
	{
		digitalWrite(pin, status);
		active = status;
	}
};

class NoResetActuator : public Actuator
{
protected:
	int pin;
public:
	inline
	NoResetActuator (int pin) : pin(pin)
	{
		setup();
	};
	virtual inline
	~NoResetActuator ()
	{}
	inline void
	setup()
	{
		pinMode(pin, OUTPUT);
		Actuator::setup();
	}
	inline void
	actuate(bool status)
	{
		digitalWrite(pin, status);
		active = status;
	}
};

class InvertedActuator : public Actuator
{
protected:
	int pin;
public:
	inline
	InvertedActuator(int pin) : pin(pin)
	{
		setup();
	};
	virtual inline
	~InvertedActuator()
	{
		actuate(false);
	}
	inline void
	setup()
	{
		pinMode(pin, OUTPUT);
		Actuator::setup();
	}
	inline void
	actuate(bool status)
	{
		digitalWrite(pin, !status);
		active = status;
	}
};

class Led : public PinActuator
{
public:
	Led(int pin) : PinActuator(pin){};
};

class Relaisswitch : public InvertedActuator
{
public:
	inline
	Relaisswitch(int pin) : InvertedActuator(pin){};
	
	inline void
	actuate(bool status) override
	{
		InvertedActuator::actuate(status);
		fprintf(stderr, "Toggled relais %d %s\n", pin, status ? "ON" : "OFF");
	}
};

class Heater : public Relaisswitch
{
	bool heating = false;
	Led* statusLed = nullptr;
	clock_t startedUp;
public:
	inline
	Heater(int pin) : Relaisswitch(pin)
	{
		Relaisswitch::actuate(true);
		active = false;
		startedUp = clock();
	}
	inline
	Heater(int pin, Led* statusLed) : Relaisswitch(pin), statusLed(statusLed)
	{
		Relaisswitch::actuate(true);
		active = false;
	}
	inline
	~Heater()
	{
		if(active)
		{
			actuate(false);
			fprintf(stderr, "Waiting for Heater to cool down...\n");
			delay(15000);
		}
	}
	inline void
	actuate(bool status) override
	{
		fprintf(stderr, "Toggled heater %s\n", status ? "ON" : "OFF");
		if(status)
		{
			float timeSinceStartedMsec = (float)(clock()-startedUp) / (CLOCKS_PER_SEC * 1000.);
			if(timeSinceStartedMsec < 800)
			{
				delay(800 - timeSinceStartedMsec);
			}
		}
		if(status != active)
		{
			system("irsend SEND_ONCE HEATER ONOFF");
			if(status)
			{
				delay(500);
				system("irsend SEND_ONCE HEATER UP");
			}
			active = status;
			if(statusLed != nullptr)
			{
				statusLed->actuate(status);
			}
		}
	}
};
	
class Tempcontrol
{
	Actuator* heat;
	Actuator* vent;
	
public:
	inline
	Tempcontrol(Actuator* heater, Actuator* ventilator) : heat(heater), vent(ventilator)
	{};
	void
	calcActions(const TempHumid& ist, const TempHumid& soll);
};