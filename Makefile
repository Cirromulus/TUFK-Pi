CFLAGS := $(CFLAGS) -ggdb -std=c++11

all: logger control

dht22.o: dht22.cpp
	g++ $(CFLAGS) -c dht22.cpp -lwiringPi

actuators.o: actuators.cpp
	g++ $(CFLAGS) -c actuators.cpp -lwiringPi
	
logger: logger.cpp dht22.o
	g++ $(CFLAGS) -o logger dht22.o logger.cpp -lwiringPi

tinyxml2.o: tinyxml2/tinyxml2.cpp
	make -C tinyxml2 staticlib

control: control.cpp dht22.o actuators.o tinyxml2.o
	g++ $(CFLAGS) -o controller dht22.o actuators.o control.cpp -Ltinyxml2 -lwiringPi -ltinyxml2

test: test.cpp dht22.o actuators.o
	g++ $(CFLAGS) -o tester dht22.o actuators.o test.cpp -lwiringPi

install:
	cp controller ..
	sudo cp -r etc/lirc/* /etc/lirc/
	sudo systemctl restart lircd.service
	sudo cp etc/controller.service /etc/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable controller.service
	sudo systemctl start controller.service
