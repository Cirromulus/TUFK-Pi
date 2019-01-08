#pragma once

#include "temptypes.hpp"

#include <wiringPi.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

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
	time_t started;
	bool bootupElapsed = false;
	static constexpr uint32_t magicStartupTime = 5; //actually 10, but safety first...
	static constexpr uint32_t magicCooloffTime = 31; //actually 10, but safety first...
public:
	
	Heater(int pin) : Relaisswitch(pin)
	{
		init();
	}
	
	inline
	Heater(int pin, Led* statusLed) : Relaisswitch(pin), statusLed(statusLed)
	{
		init();
	}
	
	inline void
	init()
	{
		Relaisswitch::actuate(true);
		active = false;
		started = time(NULL);
	}
	
	inline void
	blinkDelay(unsigned millis)
	{
		if(statusLed == nullptr)
		{
			delay(millis);
		}
		else
		{
			unsigned delayed = 0;
			while(delayed < millis)
			{
				statusLed->actuate(true);
				delay(500);
				statusLed->actuate(false);
				delay(500);
				delayed += 1000;
			}
		}
	}
	
	inline
	~Heater()
	{
		if(active)
		{
			actuate(false);
			fprintf(stderr, "Waiting %u seconds for Heater to cool down... ", (unsigned) magicCooloffTime);
			fflush(stderr);
			blinkDelay(magicCooloffTime * 1000);
			fprintf(stderr, "Done\n");
		}
	}
	
	void
	actuate(bool newState) override;
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
