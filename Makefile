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

test: test.cpp dht22.o actuators.o
	g++ $(CFLAGS) -o tester -std=c++11 dht22.o actuators.o test.cpp -lwiringPi

tinyxml:
	make -C tinyxml2 staticlib

install:
	cp controller ..
	sudo cp -r etc/lirc/* /etc/lirc/
	sudo systemctl restart lircd.service
	sudo cp etc/controller.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable controller.service
	sudo systemctl start controller.service
