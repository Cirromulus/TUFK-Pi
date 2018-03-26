all: logger control

dht: dht22.cpp
	gcc -c -std=c++11 dht22.cpp -lwiringPi

logger: logger.cpp dht
	gcc -o logger -std=c++11 dht22.o logger.cpp -lwiringPi

control: 
	