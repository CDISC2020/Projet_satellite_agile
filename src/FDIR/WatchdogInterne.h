#ifndef DEF_WDI
#define DEF_WDI

using namespace std;

class WatchdogInterne
{
private:
	int valeur;
public:
	void set();
	int readw();
	void dec();
	WatchdogInterne();
};

#endif
