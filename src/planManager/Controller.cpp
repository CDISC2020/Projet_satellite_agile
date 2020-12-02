#include "Controller.h"

using namespace std;

Controller::Controller()
{
	sc.code=2;
	sc.returnControl=true;
}

void Controller::executeInstruction(GenericInstruction* instruction, int* responseController)
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
		string path="src/communication/toSend/" + (string) C.photoName; // where to save pictures
                sc.returnControl = myCameraController.photoShoot(path, C.exposure);
		if(sc.returnControl)
		{
			//call traitement image
			/*char cmde[100]="";
			remove("res_image_processing.txt")
			sprintf(cmde,"python image_processing.py %s &", path.c_str());
			system(cmde);
			sleep(3);
			ifstream file;
			file.open("res_image_processing.txt");
			if(!file){
				cout << "No response from image processing" << endl;
				*responseController=1; // Let the Ground choose if ok
			}
			else
			{
				string line;
				getline(file,line);
				if(atoi(line.c_str()))
					*responseController=1;
				else
					*responseController=-1;
				file.close();
				remove("res_image_processing.txt")
			}
			*/

			// by pass algo, algo expensive to execute, never really tested
			// remove this line if the above isn't commented
			*responseController=1;
			//////
		}
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
}
