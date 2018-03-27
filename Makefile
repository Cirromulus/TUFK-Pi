CFLAGS := $(CFLAGS) -ggdb

all: logger control

dht: dht22.cpp
	g++ $(CFLAGS) -c -std=c++11 dht22.cpp -lwiringPi

actuators: actuators.cpp
	g++ $(CFLAGS) -c -std=c++11 actuators.cpp -lwiringPi
	
logger: logger.cpp dht
	g++ $(CFLAGS) -o logger -std=c++11 dht22.o logger.cpp -lwiringPi

control: control.cpp dht actuators
	g++ $(CFLAGS) -o controller -std=c++11 dht22.o actuators.o control.cpp -lwiringPi
	
