#ifndef CAMERA_CONTROLLER
#define CAMERA_CONTROLLER


#include <iostream>
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include <sstream>


using namespace std;

typedef struct Camera Camera;
struct Camera {
	int code;
	char photoName[64];
	int exposure;
};

class CameraController 
{
public:
	CameraController();	
	bool photoShoot(string fileName, int exposure);
};

#endif
