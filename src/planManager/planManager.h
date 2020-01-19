#ifndef PLANMANAGER
#define PLANMANAGER


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <unistd.h>

#include "plan.h"
#include "genericInstruction.h"
#include "attitudeController.h"
#include "cameraController.h"
#include "Controller.h"
#include "../ARINC/ARINC_Com.h"
#include "../communication/statusManager.h"
#include "../FDIR/FDIR.h"

using namespace std;

#define FILEPATH_SIZE 100

typedef struct PlanName PlanName;
struct PlanName {
	int code;
	char name[9]; // plan.txt
};

typedef struct PlanFilePath PlanFilePath;
struct PlanFilePath {
	int code;
	char filepath[FILEPATH_SIZE];
};

class PlanManager
{
protected:
	Plan* plan;
	int ptInstruction;
	bool bannedInstructions[100]={false};
	int responseController;

public:
	PlanManager();
	void executePlan(Controller* , QueuingPort*, bool);
	void generatePlan(const char*);
	void printPlan();
	void pushBan(int index);
	void destructPlan();
	Plan* backup();
	int getNInstru();
	void recover(Plan* planBack);
};

#endif
