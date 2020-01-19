#ifndef STATUSMANAGER
#define STATUSMANAGER
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <time.h>

using namespace std;

const int MAX_NOTIFICATION = 1000;

typedef struct Status Status;
struct Status {
	int code;
	int errorID;
	char description[128];
};

typedef struct statusControl statusControl;
struct statusControl {
	int code;
	bool returnControl;
};

class StatusManager
{
protected:
	string pathLogFile;

public:
	StatusManager();
	StatusManager(string);
	void newNotification(int errorID, string description);
};

#endif
