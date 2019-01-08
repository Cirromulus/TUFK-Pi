#include <wiringPi.h>
#include <mcp23017.h>

#include "dht22.hpp"
#include "actuators.hpp"
#include "sensors.hpp"
#include "config.hpp"

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <functional>

using namespace std;

bool done = false;

void term(int signum)
{
	fprintf(stderr, "Received signal %d\n", signum);
	fprintf(stderr, "\nCycle through loop to terminate normally, plz!\n\n");
	if(done)
	{
		fprintf(stderr, "Received signal %d AGAIN!!!!!!\n", signum);
		exit(-1);
	}
	done = true;
}

int main (int argc, char *argv[])
{
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	sigaction(SIGINT, &action, NULL);
	
	
	if (wiringPiSetup () == -1)
	{
		exit(EXIT_FAILURE);
	}
	mcp23017Setup (EXPANDER_BASE_PIN, EXPANDER_ADDR) ;

	NoResetActuator beeper(ALARMPIN);
	Led white(WHITELED);
	Led red(REDLED);
	Led green(GREENLED);
	Relaisswitch vent(VENTILATORPIN);
	Heater heat(HEATERPIN, &red);
	Relaisswitch fridge(FRIDGEPIN);
	Relaisswitch pa(PAAMPPIN);

	SmokeDetector smokeDetector(GASSENS);
	Sensor pir(PIRPIN);
	TempHumid curr;
	int tempValid;

	while(!done)
	{
		tempValid = read_dht22_dat(curr, DHT22PIN);
		cout << "PIR " << pir.getValue() << " GASSENS " << smokeDetector.getValue() << " TEMPHUMID " << (tempValid == 0 ? curr.temp : -1) << "°C " << (tempValid == 0 ? curr.humid : -1) << "rHel" << endl;
		heat.actuate(true);
		cout << "HEATER " << HEATERPIN << endl;
		getchar();
		tempValid = read_dht22_dat(curr, DHT22PIN);
		cout << "PIR " << pir.getValue() << " GASSENS " << smokeDetector.getValue() << " TEMPHUMID " << (tempValid == 0 ? curr.temp : -1) << "°C " << (tempValid == 0 ? curr.humid : -1) << "rHel" << endl;
		heat.actuate(false);
		vent.actuate(true);
		cout << "VENTILATOR " << VENTILATORPIN << endl;
		getchar();
		tempValid = read_dht22_dat(curr, DHT22PIN);
		cout << "PIR " << pir.getValue() << " GASSENS " << smokeDetector.getValue() << " TEMPHUMID " << (tempValid == 0 ? curr.temp : -1) << "°C " << (tempValid == 0 ? curr.humid : -1) << "rHel" << endl;
		vent.actuate(false);
		fridge.actuate(true);
		cout << "FRIDGE " << FRIDGEPIN << endl;
		getchar();
		tempValid = read_dht22_dat(curr, DHT22PIN);
		cout << "PIR " << pir.getValue() << " GASSENS " << smokeDetector.getValue() << " TEMPHUMID " << (tempValid == 0 ? curr.temp : -1) << "°C " << (tempValid == 0 ? curr.humid : -1) << "rHel" << endl;
		fridge.actuate(false);
		pa.actuate(true);
		cout << "PAAMP " << PAAMPPIN << endl;
		getchar();
		pa.actuate(false);
	}
	
	return 0 ;
}
