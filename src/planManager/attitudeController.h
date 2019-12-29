#ifndef ATTITUDE_CONTROLLER
#define ATTITUDE_CONTROLLER


#include <iostream>
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include <sstream>

using namespace std;

typedef struct Attitude Attitude;
struct Attitude {
	int code;
	int yaw;
	int pitch;
	int roll;
};

class AttitudeController 
{
public:
	AttitudeController();	
	bool attitudeChange(int roll, int pitch, int yaw); // roll(not used), pitch, yaw
};


#endif
