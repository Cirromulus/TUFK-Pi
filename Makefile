all: logger control

dht: dht22.cpp
	g++ -c -std=c++11 dht22.cpp -lwiringPi

actuators: actuators.cpp
	g++ -c -std=c++11 actuators.cpp -lwiringPi
	
logger: logger.cpp dht
	g++ -o logger -std=c++11 dht22.o logger.cpp -lwiringPi

control: control.cpp dht actuators
	g++ -o controller -std=c++11 dht22.o actuators.o control.cpp -lwiringPi
	