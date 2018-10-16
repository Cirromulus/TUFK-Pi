#pragma once

#include "temptypes.hpp"

#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

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
	bool justStartedUp = true;
public:
	inline
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
		justStartedUp = true;
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
			fprintf(stderr, "Waiting for Heater to cool down... ");
			fflush(stderr);
			//blinkDelay(31000);
			blinkDelay(5000);
			fprintf(stderr, "Done\n");
		}
	}
	inline void
	actuate(bool status) override
	{
		fprintf(stderr, "Toggled heater %s\n", status ? "ON" : "OFF");
		if(status && justStartedUp)
		{
			fprintf(stderr, "Waiting for Heater to start up... ");
			fflush(stderr);
			blinkDelay(10500);
			fprintf(stderr, "Done\n");
			justStartedUp = false;
		}
		if(status != active)
		{
			system("irsend SEND_START HEATER ONOFF");
			delay(250);
			system("irsend SEND_STOP HEATER ONOFF");
			if(status)
			{
				system("irsend SEND_START HEATER UP");
				delay(150);
				system("irsend SEND_STOP HEATER UP");
				delay(250);
				system("irsend SEND_START HEATER DOWN");
				delay(150);
				system("irsend SEND_STOP HEATER DOWN");
				delay(250);
				system("irsend SEND_START HEATER SWING");
				delay(150);
				system("irsend SEND_STOP HEATER SWING");
				//delay(250);
				//system("irsend SEND_ONCE HEATER MODE");
				//delay(250);
				//system("irsend SEND_ONCE HEATER MODE");
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
