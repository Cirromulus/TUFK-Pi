#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#include "temptypes.hpp"
#pragma once

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

class Led : public Actuator
{
protected:
	int pin;
public:
	inline
	Led(int pin) : pin(pin)
	{
		setup();
	};
	inline
	~Led()
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

class Relaisswitch : public Actuator
{
	int pin;
public:
	inline
	Relaisswitch(int pin) : pin(pin)
	{
		setup();
	};
	inline
	~Relaisswitch()
	{
		actuate(false);
	};
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
		fprintf(stderr, "Toggled relais %d %s\n", pin, status ? "ON" : "OFF");
	}
};

class Heater : public Relaisswitch
{
	bool heating = false;
	Led* statusLed = nullptr;
public:
	inline
	Heater(int pin) : Relaisswitch(pin)
	{
		Relaisswitch::actuate(true);
		active = false;
	}
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
		Relaisswitch::actuate(false);
	}
	inline void
	actuate(bool status)
	{
		fprintf(stderr, "Toggled heater %s\n", status ? "ON" : "OFF");
		if(status != active)
		{
			system("irsend SEND_ONCE HEATER ONOFF");
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