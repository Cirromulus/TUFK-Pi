CFLAGS := $(CFLAGS) -ggdb

all: logger control

dht: dht22.cpp
	g++ $(CFLAGS) -c -std=c++11 dht22.cpp -lwiringPi

actuators: actuators.cpp
	g++ $(CFLAGS) -c -std=c++11 actuators.cpp -lwiringPi
	
logger: logger.cpp dht22.o
	g++ $(CFLAGS) -o logger -std=c++11 dht22.o logger.cpp -lwiringPi

control: control.cpp dht22.o actuators.o tinyxml
	g++ $(CFLAGS) -o controller -std=c++11 dht22.o actuators.o control.cpp -Ltinyxml2 -lwiringPi -ltinyxml2

tester: test.cpp dht22.o actuators.0
	g++ $(CFLAGS) -o tester -std=c++11 dht22.o actuators.o test.cpp -lwiringPi

tinyxml:
	make -C tinyxml2 staticlib