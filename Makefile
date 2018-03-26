all: logger

logger: logger.cpp
	gcc -o logger -std=c++11 logger.cpp -lwiringPi
