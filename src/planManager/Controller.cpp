#include "Controller.h"

using namespace std;

Controller::Controller()
{
	sc.code=2;
	sc.returnControl=true;
}

void Controller::executeInstruction(GenericInstruction* instruction, int* responseController, QueuingPort* ChannelErreur, Plan* P, int ptInstruction)
{
	if(instruction->getType() == 'p')
	{
		Camera C;
		C.code=1;
		C.exposure = ((PhotoInstruction*)instruction)->getExposure();
		string aux = ((PhotoInstruction*)instruction)->getPhotoName();
		strcpy(C.photoName,aux.c_str());

		cout << endl << "New photo! Smile!" << endl;
		cout<<"name ="<<C.photoName<<"    exposure = "<<C.exposure<<endl;
                sc.returnControl = myCameraController.photoShoot((string) C.photoName, C.exposure);
		if(sc.returnControl)
			*responseController=1;
		else
			*responseController=-1;
	}
	else if(instruction->getType() == 'a')
	{
		Attitude A; 
		A.code = 0; 
		A.yaw = ((AttitudeInstruction*)instruction)->getYaw(); 
		A.pitch = ((AttitudeInstruction*)instruction)->getPitch(); 
		A.roll = ((AttitudeInstruction*)instruction)->getRoll(); 

		cout << endl << "New attitude change!!" << endl;
		cout<<"yaw ="<<A.yaw<<"    pitch = "<<A.pitch<<"    roll = "<<A.roll<<endl;
                sc.returnControl = myAttitudeController.attitudeChange(A.roll, A.pitch, A.yaw);
		if(sc.returnControl)
			*responseController=1;
		else
			*responseController=-1;
	}

	else
	{
		Status S;
		S.code = 4;
		S.errorID = 2; // A changer.
		sprintf(S.description, "Wrong instruction type, Plan %d.%d : Line #%d : Index : #%d ",P->getID(),P->getVersion(), ptInstruction+1,  instruction->getIndex());

		ChannelErreur->SendQueuingMsg((char*)&S, sizeof(Status));
	}
}
