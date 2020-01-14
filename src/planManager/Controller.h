#ifndef DEF_CONTROLLER
#define DEF_CONTROLLER

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#include "../ARINC/ARINC_Com.h"
#include "plan.h"
#include "genericInstruction.h"
#include "cameraController.h"   
#include "attitudeController.h"
#include "../communication/statusManager.h"

class Controller
{
protected:
	CameraController myCameraController;
	AttitudeController myAttitudeController;

	Attitude* a;
	Camera* c;
	statusControl sc;

public:
	Controller();
	void executeInstruction(GenericInstruction* instruction, int* responseController, QueuingPort* ChannelErreur, Plan* P, int ptInstruction);
};

#endif
