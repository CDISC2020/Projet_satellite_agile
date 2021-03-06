#ifndef DEF_FDIR
#define DEF_FDIR

#include "WatchdogInterne.h"
#include "watchdog_arduino.h"
#include "../GPIO.h"

using namespace std;

typedef struct ModeStruct ModeStruct;
struct ModeStruct {
	int code;
	bool rpiMode;
};

class FDIR
{
	private:
		//void Launch_Com_Backup();
		//void Launch_Plan_Backup();

		bool leader;
		int nbr_arduino_vide;
		int nbr_reboot_com;
		int nbr_reboot_plan;


		WatchdogInterne watch_com;
		WatchdogInterne watch_plan;
		Watchdog_arduino watch_ard;


	public:
		FDIR();
		void set_watch_com();
		void set_watch_plan();
			
		void dec_watch_com();
		void dec_watch_plan();

		int read_watch_com();
		int read_watch_plan();

		void set_arduino();
		void reset_arduino();
		int read_arduino();		

		bool isleader();
		void switch_leader();
};

#endif
