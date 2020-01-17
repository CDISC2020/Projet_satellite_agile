#include "WatchdogInterne.h"

using namespace std;



WatchdogInterne::WatchdogInterne()
{

}


void WatchdogInterne::set(){
	// I'm alive!!!!
	valeur = 5;
}


int WatchdogInterne::readw() 
{
	return valeur;
}

void WatchdogInterne::dec()
{
	valeur = valeur-1;
}

