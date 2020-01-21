#include <iostream>
//#include <string.h>
#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <pthread.h>
#include <dirent.h>
#include <vector>
#include <algorithm>

using namespace std;

#include "../ARINC/ARINC_Com.h"
#include "statusManager.h"
#include "../planManager/planManager.h"
#include "../FDIR/FDIR.h"

/*-------------------------------VARIABLES--------------------------------------*/
int tid_sol, tid_interne, tid_alive;
StatusManager sm;
Status *status;
PlanName *p;
ModeStruct *m;
PlanFilePath *imageName;
string imageList[128];
int ptImageReceived = 0;
int ptImageSent = 0;
bool mode = false; // Mode slave; // toujours init a false
char s[100];

char buffer[1024];
char cmde[100];

int wtc = 0;

//mutex sut tableau list_images

/*---------------------------------FONCTIONS AUX--------------------------------------*/
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

bool tryFile (string file_dir, string file_name)
{
	string path = file_dir + file_name;
	fstream filestr;
 	filestr.open(path.c_str());

	if (filestr.is_open())
	{
    		filestr.close();
		return true;
  	}
	else
	{
		cout << "Error to open file" << endl;
		Status S;
		S.errorID = 9; // A changer.
		sprintf(S.description, "File corrupted");
		string str(S.description);
		sm.newNotification(S.errorID, str);
		return false;
	}
}

/*---------------------------------COMMUNICATION AU SOL--------------------------------------*/
void* Communic_Sol(void* args)
{
	QueuingPort channelOutPM(0, 18001, s); 	// Client PM
	usleep(500000);
	channelOutPM.Display();

    	vector<string> files, tms;
	vector<string>::iterator it;
	ifstream ifile;

	PlanFilePath pfp_plan, pfp_tm;
	pfp_plan.code = 3;
	pfp_tm.code = 17;

	// Modifiable
	// Dossiers reception
	string dir_plan = string("src/communication/planRecuSol/");
	string dir_tm = string("src/communication/tmRecuSol/");

	// Fichier a chercher
	string name_dem = "demande_imgs.txt";
	string name_plan = "plan.txt";
	string name_tm = "tm.txt";

	// Dossier une fois traité
	string c_plan = "src/planManager/plans/";
	string c_tm = "src/planManager/tm/";

	bool verification_plan;
	bool verification_tm;
	
	while(1)
	{
		if(mode == true)
		{
			files.clear();
    			getdir(dir_plan,files);	

			/*****************************[ CGM ---IMGS---> SOL ]**********************************/
			it=find(files.begin(),files.end(),name_dem);
			if(it!=files.end()) // Si fichier présent dans la liste
			{
				cout << "File find" << endl;

				sprintf(cmde, "sh src/communication/uploadStoG.sh LogError.txt");
				system(cmde);

				while(ptImageSent != ptImageReceived)
				{

					// envoyer les images existentes
					sprintf(cmde, "sh src/communication/uploadStoG.sh %s", imageList[ptImageSent].c_str()); 
					system(cmde);
					sleep(1);
					cout << "Image sent \n" << endl;

					//enlever du satellite l'image envoyée au sol
					sprintf(cmde, "rm %s", imageList[ptImageSent].c_str());
					system(cmde);
					cout << "Image removed\n" << endl;

					ptImageSent = (ptImageSent + 1)%128;

				}// lancer bash qui envoie chaque photo du tableau.
				string rep_demande_img= dir_plan + name_dem;
				sleep(5);
				remove(rep_demande_img.c_str());
				cout << "Demande enlevée\n";
				sleep(5);
			}
			
			/*****************************[ CGM ---PLAN---> PM ]**********************************/
			it=find(files.begin(),files.end(),name_plan);
			if(it!=files.end()) // Si fichier présent dans la liste
			{
 				verification_plan = tryFile(dir_plan, name_plan); //verifier si on reussi a ouvrir le plan

				if(verification_plan)
				{
					// deplace dans src/planMagager/plans avec system()
					sprintf(cmde, "mv %s%s %s", dir_plan.c_str(), name_plan.c_str(), c_plan.c_str());
					system(cmde);
					sleep(1);
					cout << "Plan envoyé au PM\n";

					sprintf(pfp_plan.filepath, "%s%s", c_plan.c_str(), name_plan.c_str());
					channelOutPM.SendQueuingMsg((char*)&pfp_plan, sizeof(PlanFilePath));
				}
			}

			/*****************************[ CGM ---TM---> PM ]**********************************/
			tms.clear();
    			getdir(dir_tm,tms);			
			it=find(tms.begin(),tms.end(),name_tm);
			if(it!=tms.end()) // Si fichier présent dans la liste
			{
				verification_tm = tryFile(dir_tm,name_tm);

				if(verification_tm)
				{
						// deplace dans src/planMagager/tm avec system()
					sprintf(cmde, "mv %s%s %s", dir_tm.c_str(), name_tm.c_str(), c_tm.c_str());
					system(cmde);
					sleep(1);
					cout << "TM envoyé au PM\n";

					sprintf(pfp_tm.filepath, "%s%s", c_tm.c_str(), name_tm.c_str());

					channelOutPM.SendQueuingMsg((char*)&pfp_tm, sizeof(PlanFilePath));
				}
			}
		}

//		else{
//			cout << "In follower mode\n" << endl;}

		wtc = 1;

		usleep(100);
	}

	return NULL;
}

/*----------------------------COMMUNICATION AVEC PLAN MANAGER---------------------------------*/
void* Communic_Interne(void* args)
{
	QueuingPort channelIn(1, 18003, s); 		// Server
	usleep(500000);
	channelIn.Display();
	usleep(300000);

	while(1)
	{
		channelIn.RecvQueuingMsg(buffer);

		status = (Status*)buffer;

 		/**************[ PM ---IMG---> CGM ]**************/
		// Remplir le tableau avec des photos prises
		if (status->code == 3)		// utilisation d'un type p
		{
			imageName = (PlanFilePath*)buffer;
			string aux(imageName->filepath);
			imageList[ptImageReceived] = aux;
			ptImageReceived = (ptImageReceived + 1)%128;
		}

 		/**************[ PM ---LOG---> CGM ]**************/
		// Plan manager --> Comunication Manager
		// Error identifier and description
		else if(status->code == 4)
		{
			string str(status->description);
			sm.newNotification(status->errorID, str);
		}

		/*************[ FDIR ---MODE---> CGM ]*************/
		// changement mode primary ou backup
		else if(status->code == 6)
		{
			m = (ModeStruct*)buffer;
			mode = m->rpiMode;
		}

		usleep(100);
	}

	return NULL;
}

/*---------------------------------ALIVE--------------------------------------*/
void* am_alive(void* args)
{
	QueuingPort channelFDIR(0, 18002, s); // Client FDIR
	usleep(500000);
	channelFDIR.Display();

	char str[2]="C";
	while (1) 
	{
		if(wtc == 1)
		{
			channelFDIR.SendQueuingMsg(str, sizeof(str));
			wtc = 0;
		}
		usleep(20*1000); //20 ms
	}

	return NULL;
}


/*---------------------------------BYYYE--------------------------------------*/
sig_t bye()
{
	printf("S-> Bye !\n");
	exit(0);
}

/*---------------------------------MAIN--------------------------------------*/
int main (int argc, char* argv[])
{
	// Ecriture du pid dans un fichier
	int pid=getpid();
	ofstream fichierPid;
	fichierPid.open("pid_COM");
	if(!fichierPid)	{
		cout << "Failed to open pid_COM" << endl;
		exit(1);
	}
	fichierPid << pid << endl;
	fichierPid.close();

	// Recuperation du hostname
	if (gethostname(s, 100) != 0){
		perror("S-> gethostname");
		exit(1);
	}
	cout << "Host name " << s << endl;

	// Init des tableaux
	for(int i=0; i<1024; i++) buffer[i]='\0';
	for(int i=0; i<50; i++) cmde[i]=' ';

	// Signal
	signal(SIGINT, (sig_t)bye);


	/* CREATION DES THREADS	*/
	pthread_attr_t *thread_attributes;
	pthread_t *thread;

/////////* creation du thread processus communication au sol */
	tid_sol = 1;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, &Communic_Sol, (void*) NULL) != 0)
		perror ("Thread_Server-> Connection to ground error!");

/////////* creation du thread processus communication interne */
	tid_interne=2;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, &Communic_Interne, (void *) NULL) != 0)
		perror ("Thread_Server-> Internal connection error!");

/////////* creation du thread processus I'm alive */
	tid_alive=3;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, &am_alive, (void *)  NULL) != 0)
		perror ("Thread_Server-> I am alive error!");

	while (1) {usleep(100);}  /* DOES NOTHING */ ;

	return 0;
}
