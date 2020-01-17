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

#define PLANS_BUFFER_SIZE 2

using namespace std;

typedef struct PlanName PlanName;
struct PlanName {
	int code;
	char name[11];
};

typedef struct PlanFilePath PlanFilePath;
struct PlanFilePath {
	int code;
	char filepath[64];
};

typedef struct ModeStruct ModeStruct;
struct ModeStruct {
	int code;
	bool rpiMode;
};

class PlanManager 
{
protected:
	Plan Plans[PLANS_BUFFER_SIZE];
	int nPlan;
	int ptInstruction;
	int indexPlan;
	bool bannedInstructions[100]={false};

public:
	PlanManager();
	void executePlan(Controller* , int *,QueuingPort*, bool);
	void generatePlan(const char*);
	void printPlan(int indexPlan);
	void pushBan(int index);
};

#endif
