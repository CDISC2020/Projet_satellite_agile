#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>
#include <unistd.h>
#include <fstream>
#include <linux/inotify.h>

using namespace std;

#include "../ARINC/ARINC_Com.h"
#include "statusManager.h"
#include "../planManager/planManager.h"

int  main (int argc,char* argv[]) 
{
	if (argc!=2) 
	{
		printf("T'as oublie l'argument pinpin ! Le hostname... \n");
		exit (-1);
	}

	StatusManager sm;
	Status *status;	
	PlanName *p;
	ModeStruct *m;
	PlanFilePath pfp;
	PlanFilePath *imageName;
	string imageList[128];
	int ptImageReceived = 0;
	int ptImageSent = 0;
	bool mode = true; // Mode slave;
	char s[100];

	if (gethostname(s, 100) != 0) 
	{
	    perror("S-> gethostname");
	    exit(1);
	}

	pfp.code = 3;
	//reception telecommande notif

	cout << "Host name " << s << endl; 
	
	/**QueuingPort channelOutPM(0, 18001, argv[1]); 	// Client
	QueuingPort channelIn(1, 18003, s); 		// Server	

	channelIn.Display();
	channelOutPM.Display();**/

	char buffer[1024];
	int i; for(i=0; i>1024; i++) buffer[i] = '\0';
	char cmde[] = {"                                "};
	//char file_demande = "demande_imgs.txt";
	
	while(1) 
	{
		ifstream ifile;
   		ifile.open("demande_imgs.txt");
		//channelIn.RecvQueuingMsg(buffer);*
/**
		status = (Status*)buffer;

		if (status->code == 3)		// utilisation d'un type p
		{
			imageName = (PlanFilePath*)buffer;
			string aux(imageName->filepath);
			imageList[ptImageReceived] = aux;
			ptImageReceived = (ptImageReceived + 1)%128;
		}
		else if(status->code == 4) 
		{
			string str(status->description);
			sm.newNotification(status->errorID, str);
		}
		else if(status->code == 5) 
		{
			cout << "plant recu!" << endl;
			p = (PlanName*)buffer;
			string name(p->name);
			string path="src/planManager/plans/"+name;
			for(unsigned int i=0; i<path.length(); i++)
				pfp.filepath[i] = path[i];
			cout<<p->name;
			channelOutPM.SendQueuingMsg((char*)&pfp, sizeof(PlanFilePath));
		}
		else if(status->code == 6)
		{
			m = (ModeStruct*)buffer;
			mode = m->rpiMode;
		}**/
		if( ( ifile)  && (mode == true))
		{	
			//printf("inside boucle envoi photos");
			string aux("image_test.jpg");	
			imageList[0] = aux;	
			sprintf(cmde, "sh src/communication/uploadStoG.sh LogError.txt");
			system(cmde);
			ptImageReceived=1;
			while(ptImageSent != ptImageReceived)
			{
				printf("boucle envoi photos");						
				sprintf(cmde, "sh src/communication/uploadStoG.sh %s", imageList[ptImageSent].c_str());
				system(cmde);
				sleep(1);
				sprintf(cmde, "rm %s", imageList[ptImageSent].c_str());
				system(cmde);
				ptImageSent = (ptImageSent + 1)%128;
			}// lancer bash qui envoie chaque photo du tableau.
			remove("demande_imgs.txt");
		}
		//else{ printf("non");}
	}

	return 0;
}
