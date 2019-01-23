#include <wiringPi.h>
#include <mcp23017.h>

#include "dht22.hpp"
#include "actuators.hpp"
#include "sensors.hpp"
#include "config.hpp"

#include "xml.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <functional>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <sstream>

using namespace std;

volatile sig_atomic_t done;
bool wasMovementSinceLastUpdate;
bool wasFireSinceLastUpdate;
bool wasHeaterOn;
bool wasVentOn;


void logToServer(string serverURI, const TempHumid& th)
{
	uint32_t statusBits = wasFireSinceLastUpdate << 3 |
		wasMovementSinceLastUpdate << 2 |
		wasHeaterOn << 1 |
		wasVentOn;

	try {
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		// Setting the URL to retrive. time, temp, humid, actuators
		string url(serverURI + "upload.php?");
		url += "time=";
		url += to_string((unsigned)time(NULL));
		url += "&temp=";
		url += to_string(th.temp);
		url += "&humid=";
		url += to_string(th.humid);
		url += "&actuators=";
		url += to_string(statusBits);

		request.setOpt(new curlpp::options::Url(url));
		request.perform();
		wasMovementSinceLastUpdate = false;
		wasFireSinceLastUpdate = false;
		wasHeaterOn = false;
		wasVentOn = false;
	}
	catch ( curlpp::LogicError & e ) {
		std::cout << e.what() << std::endl;
	}
	catch ( curlpp::RuntimeError & e ) {
		std::cout << e.what() << std::endl;
	}
}

bool getConfigFromServer(string serverURI, Config& config)
{
	std::ostringstream os;
	try {
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		// Setting the URL to retrive. time, temp, humid, actuators
		string url(serverURI + "config.php");

		request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::Header(0));

		os << request;
		return config.reloadFromString(os.str());
	}
	catch ( curlpp::LogicError & e ) {
		std::cout << e.what() << std::endl;
		cout << os;
	}
	catch ( curlpp::RuntimeError & e ) {
		std::cout << e.what() << std::endl;
		cout << os;
	}
	return false;
}

void logCsv(const TempHumid& th, bool isHeaterOn, bool isVentOn)
{
	printf("%u,%.2f,%.2f,%d,%d\n", (unsigned)time(NULL), th.temp, th.humid, isHeaterOn, isVentOn);
	fflush(stdout);
}

void term(int signum)
{
	fprintf(stderr, "Received signal %d\n", signum);
	if(done)
	{
		fprintf(stderr, "Received signal %d AGAIN!!!!!!\n", signum);
		exit(-1);
	}
	done = true;
}

Actuator* globalBeeper = nullptr;
Sensor* globalSmoke = nullptr;
void ohNoez()
{
	if(globalSmoke != nullptr)
	{
		//dehysterese
		printf("Smoke: %s\n", globalSmoke->getValue() ? "yes" : "no");
		if(!globalSmoke->getValue())
		{
			return;
		}
		delay(1000);
		if(!globalSmoke->getValue())
		{
			return;
		}
	}
	//FIXME This deactivates the Smokedetector.
	wasFireSinceLastUpdate = true;
	return;

	if(globalBeeper != nullptr)
	{
		globalBeeper->actuate(true);
	}
	fprintf(stderr, "Oh NOEZ, es brennt!\n");
	term(0);
	term(118);
	term(999);
	term(881);
	term(999);
	term(119);
	term(725);
	term(3);
}

//Ugly
Sensor* movementSensor = nullptr;
Led* movementLed = nullptr;
void movementChanged()
{
	if(movementSensor != nullptr)
	{
		int val = movementSensor->getValue();
		if(val)
		{
			//fprintf(stderr, "Movement detected\n");
			wasMovementSinceLastUpdate = true;
		}
		else
		{
			//fprintf(stderr, "Movement ended (possibly)\n");
		}
		if(movementLed != nullptr)
		{
			movementLed->actuate(val);
		}
	}
}

int main (int argc, char *argv[])
{
	done = false;
	wasMovementSinceLastUpdate = false;
	wasFireSinceLastUpdate = false;
	wasHeaterOn = false;
	wasVentOn = false;

	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);


	if (argc < 2)
	{
		printf ("usage: %s configfile\n\tconfigfile will be created if it does not exist\n", argv[0]);
		return -EINVAL;
	}

	Config config(argv[1]);
	unsigned long lastServerConnection = 0;

	if(!getConfigFromServer(config.getServerURI(), config))
	{
		cerr << "Invalid config from server, loading local file" << endl;
	}
	else
	{
		cout << "Server Config OK" << endl;
		lastServerConnection = time(NULL);
	}

	printf("Target: %0.2f °C, %0.2f%%\n", config.getTempHumid().temp, config.getTempHumid().humid);

	if (wiringPiSetup () == -1)
	{
		exit(EXIT_FAILURE);
	}
	mcp23017Setup (EXPANDER_BASE_PIN, EXPANDER_ADDR) ;

	NoResetActuator beeper(ALARMPIN);
	globalBeeper = &beeper;
	Led white(WHITELED);
	Led red(REDLED);
	Led green(GREENLED);
	Relaisswitch vent(VENTILATORPIN);
	Heater heat(HEATERPIN, &red);
	Relaisswitch fridge(FRIDGEPIN);
	Relaisswitch pa(PAAMPPIN);

	SmokeDetector smokeDetector(GASSENS);
	globalSmoke = &smokeDetector;
	if(smokeDetector.getValue())
	{
		beeper.actuate(true);
		fprintf(stderr, "Oh NOEZ, es brennt!\n");
		return EXIT_FAILURE;
	}
	smokeDetector.registerInterrupt(Sensor::Sensitivity::both, ohNoez);
	Sensor pir(PIRPIN);
	movementSensor = &pir;
	movementLed = &green;
	pir.registerInterrupt(Sensor::Sensitivity::both, movementChanged);


	Tempcontrol tempcontrol(&heat, &vent);
	TempHumid curr, last;

	while (!done)
	{
		int ret = read_dht22_dat(curr, DHT22PIN);
		if(ret < 0)
		{
			continue;
		}
		if(curr.temp < -10)
		{
			//Hopefully we never reach this
			//Unterdrücke fehlerhafte samples
			continue;
		}

		tempcontrol.calcActions(curr, config);
		wasHeaterOn |= heat.getStatus();
		wasVentOn |= vent.getStatus();
		if(time(NULL) > lastServerConnection + config.getServerConnectPeriod())
		{
			logToServer(config.getServerURI(), curr);
			lastServerConnection = time(NULL);

			if(!getConfigFromServer(config.getServerURI(), config))
			{
				cerr << "Invalid config from server, using last valid" << endl;
			}
			else
			{
				cout << "Server Config OK" << endl;
			}
		}
		logCsv(curr, heat.getStatus(), vent.getStatus());
		last = curr;

		white.actuate(false);
		unsigned delayed = 0;
		unsigned msdelay = config.getSamplingPeriod() * 1000;
		while(delayed < msdelay)
		{
			if(done)
			{
				break;
			}
			delay(250);
			delayed += 250;
		}
		white.actuate(true);
	}

	return 0 ;
}
