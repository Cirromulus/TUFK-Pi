CFLAGS := $(CFLAGS) -ggdb -std=c++11
CC = g++
#arm-linux-gnueabihf-g++


all: logger control

dht22.o: dht22.cpp
	$(CC) $(CFLAGS) -c dht22.cpp -lwiringPi

actuators.o: actuators.cpp
	$(CC) $(CFLAGS) -c actuators.cpp -lwiringPi

tinyxml2/tinyxml2.o: tinyxml2/tinyxml2.cpp
	make -C tinyxml2 staticlib

xml.o: xml.cpp tinyxml2/tinyxml2.o
	$(CC) $(CFLAGS) -c xml.cpp

logger: logger.cpp dht22.o xml.o
	$(CC) $(CFLAGS) -o logger dht22.o logger.cpp -lwiringPi

controller: control.cpp dht22.o actuators.o xml.o
	$(CC) $(CFLAGS) -o controller dht22.o actuators.o xml.o control.cpp -lwiringPi -lcurlpp -lcurl -ltinyxml2 -Ltinyxml2

tester: test.cpp dht22.o actuators.o xml.o
	#todo: remove xml abhängigkeit
	$(CC) $(CFLAGS) -o tester dht22.o actuators.o xml.o test.cpp -lwiringPi -ltinyxml2 -Ltinyxml2

init:
	sudo cp -r etc/lirc/* /etc/lirc/
	sudo systemctl restart lircd.service
	sudo cp etc/controller.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable controller.service

install: control
	-sudo systemctl stop controller.service
	cp controller ..
	sudo systemctl start controller.service
