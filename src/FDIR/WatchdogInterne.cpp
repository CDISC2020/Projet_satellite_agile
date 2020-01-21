#include "WatchdogInterne.h"

using namespace std;



WatchdogInterne::WatchdogInterne()
{
	valeur_max=5;
	valeur=valeur_max;
}


void WatchdogInterne::set(){
	// I'm alive!!!!
	valeur = valeur_max;
}


int WatchdogInterne::readw() 
{
	return valeur;
}

void WatchdogInterne::dec()
{
	valeur = valeur-1;
}

