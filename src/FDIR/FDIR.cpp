#include "FDIR.h"
#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1
using namespace std;


FDIR::FDIR() 
{
	leader=false;
}

//on peut optimiser le nombre de méthodes en donnant un attribut aux différents WD, si jamais on a le temps

void FDIR::set_watch_com()
{
	watch_com.set();
}

void FDIR::dec_watch_com()
{
	watch_com.dec();
}

void FDIR::set_watch_plan()
{
	watch_plan.set();
}

void FDIR::dec_watch_plan()
{
	watch_plan.dec();
}

int FDIR::read_watch_com()
{
	return watch_com.readw();
}

int FDIR::read_watch_plan()
{
	return watch_plan.readw();
}

void FDIR::set_arduino()
{
	watch_ard.set();
}

void FDIR::reset_arduino()
{
	watch_ard.reset();
}
int FDIR::read_arduino()
{
	//attention quand la raspi d'en face est vivante, la gpio est a 0. Logique inverse d'où le "!" ! 
	return (!watch_ard.readw());
}

bool FDIR::isleader()
{
	return leader;
}

void FDIR::switch_leader()
{
	leader=!leader;
}
