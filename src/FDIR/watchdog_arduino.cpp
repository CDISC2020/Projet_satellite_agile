#include "watchdog_arduino.h"

using namespace std;

#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1

Watchdog_arduino::Watchdog_arduino() 
{
	// activation des GPIO
	GPIOExport(3);
	GPIOExport(2);

	// choix du mode (in/out) des GPIO
	GPIODirection(3,OUT);
	GPIODirection(2,IN);
}


void Watchdog_arduino::set() 
{
	GPIOWrite(3,HIGH);
}

void Watchdog_arduino::reset()
{
	GPIOWrite(3,LOW);
}


int Watchdog_arduino::readw() 
{
	// Lecture de l'état du GPIO
	return GPIORead(2);
}
