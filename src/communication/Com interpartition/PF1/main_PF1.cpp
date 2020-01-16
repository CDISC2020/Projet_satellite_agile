#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

#include "../ARINC/ARINC_Com.h"
#include "statusManager.h"
#include "../planManager/planManager.h"

PlanFilePath pfp;

int main (int argc,char* argv[]) 
{
cout<<"Je suis le PF1, respo com"<<endl;

if (argc!=2) 
{
	printf("T'as oublié l'argument banane!le hostname... \n");
	exit (-1);
}

// Suite si hostname en argument

cout << "Host name " << argv[1] << endl; 

QueuingPort channelOutPM(0, 18001, argv[1]);

channeloutPM.Display();

char str[100]="C";

while (1) {

	//channel.SendQueuingMsg(str, sizeof(str));	
	
	//sleep(3);
			string dir = string("./TestSatelliteSaturne");
    			vector<string> files = vector<string>();

    			getdir(dir,files);

    			files.erase(files.begin());
    			files.erase(files.end());
			
		    	for (unsigned int i = 0;i < files.size();i++) {
				char c=files[i][0];				
				pfp.filepath[i] = c;
		    	}
			cout << pfp.filepath << "buffer pfp";
			channelOutPM.SendQueuingMsg((char*)&pfp, sizeof(PlanFilePath));

}

}
