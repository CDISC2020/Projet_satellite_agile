#ifndef WATCHDOG_ARDUINO
#define WATCHDOG_ARDUINO


#include <iostream>
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../GPIO.h"

using namespace std;

class Watchdog_arduino
{
public:
	Watchdog_arduino();
	void set();
	void reset();
	int readw();
};

#endif
