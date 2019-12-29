#ifndef WATCHDOG
#define WATCHDOG


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

class Watchdog 
{
public:
	Watchdog();
	void set();
	int readw();
};

#endif
