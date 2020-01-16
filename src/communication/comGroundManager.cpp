#include <iostream>
//#include <string.h>
#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
//#include <conio.h>
#include <unistd.h>
#include <fstream>
#include <linux/inotify.h>
#include <pthread.h>
#include <dirent.h>
#include <vector>

using namespace std;

#include "../ARINC/ARINC_Com.h"
#include "statusManager.h"
#include "../planManager/planManager.h"

/*-------------------------------VARIABLES--------------------------------------*/
int tid_sol, tid_interne;
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

char buffer[1024];
char cmde[50];
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

/*---------------------------------COMMUNICATION AU SOL--------------------------------------*/
void* Communic_Sol(void *args)
{
	while(1)
	{
		if(mode == true)
		{
			ifstream ifile;
	   		ifile.open("demande_imgs.txt");
			if((ifile))
			{
				cout << "File find" << endl;
				cout << "mode true" << endl;
				string aux = "image_test.jpg";
				imageList[0] = aux;

				//sprintf(cmde, "sh src/communication/uploadStoG.sh, "LogError.txt");
				//system(cmde);

				ptImageReceived=1;

				cout << "pt sent " << ptImageSent << endl;


				while(ptImageSent != ptImageReceived)
				{
					//cout << "boucle \n";
					// envoyer les images existentes
					sprintf(cmde, "sh src/communication/uploadStoG.sh %s", imageList[ptImageSent].c_str());

					system(cmde);

					sleep(1);
					cout << "Img envoyée " << endl;

					//enlever du satellite l'image envoyée au sol
					sprintf(cmde, "rm %s", imageList[ptImageSent].c_str());

					system(cmde);
					cout << "Images enlevées" << endl;

					ptImageSent = (ptImageSent + 1)%128;

				}// lancer bash qui envoie chaque photo du tableau.
				remove("demande_imgs.txt");
				cout << "Demande enlevée\n";
			}
			else
				cout << "Can't open file" << endl;
		}

		else 
			cout << "In follower mode" << endl;
	}
}

/*----------------------------COMMUNICATION INTER-PARTITIONS---------------------------------*/
void* Communic_Interne(void* argv)
{
	char* argv_char = static_cast<char*>(argv);

	QueuingPort channelOutPM(0, 18001, argv_char); 	// Client PM
	QueuingPort channelIn(1, 18003, s); 		// Server

	channelIn.Display();
	channelOutPM.Display();

	while(1)
	{
		channelIn.RecvQueuingMsg(buffer);

		status = (Status*)buffer;

		// Remplir le tableau avec des photos prises

		if (status->code == 3)		// utilisation d'un type p
		{
			imageName = (PlanFilePath*)buffer;
			string aux(imageName->filepath);
			imageList[ptImageReceived] = aux;
			ptImageReceived = (ptImageReceived + 1)%128;
		}

		// Plan manager --> Comunication Manager
		// Error identifier and description

		else if(status->code == 4)
		{
			string str(status->description);
			sm.newNotification(status->errorID, str);
		}

		// Recuperer le nom du plan envoyé par le sol
		else if(status->code == 5)
		{
			string dir = string(".");
    			vector<string> files = vector<string>();

    			getdir(dir,files);

    			files.erase(files.begin()); // ??!
    			files.erase(files.end());   // ??!

		    	for (unsigned int i = 0;i < files.size();i++) {
				char c=files[i][0];
				pfp.filepath[i] = c;
		    	}
			cout << pfp.filepath << "buffer pfp";
			channelOutPM.SendQueuingMsg((char*)&pfp, sizeof(PlanFilePath));
		}

		// changement mode primary ou backup

		else if(status->code == 6)
		{
			m = (ModeStruct*)buffer;
			mode = m->rpiMode;
		}

		else if((status->code == 10) && (mode == true))
		{
			sprintf(cmde, "sh uploadStoG.sh LogError.txt");
			system(cmde);
			while(ptImageSent != ptImageReceived)
			{
				sprintf(cmde, "sh uploadStoG.sh %s", imageList[ptImageSent].c_str());
				system(cmde);
				sleep(1);
				sprintf(cmde, "rm %s", imageList[ptImageSent].c_str());
				system(cmde);
				ptImageSent = (ptImageSent + 1)%128;
			}// lancer bash qui envoie chaque photo du tableau.
		}

	}
}

/*---------------------------------BYYYE--------------------------------------*/
sig_t bye()
{
  printf("S-> Salut !\n");
  exit(0);
}

/*---------------------------------MAIN--------------------------------------*/
int main (int argc, char* argv[])
{
	if (argc!=2)
	{
		printf("T'as oublie l'argument pinpin ! Le hostname... \n");
		exit (-1);
	}

	if (gethostname(s, 100) != 0)
	{
	    perror("S-> gethostname");
	    exit(1);
	}

	void *argv_void = static_cast<void*>(argv[1]);

	int i; for(i=0; i>1024; i++) buffer[i] = '\0';
	
	for(int i=0; i<50; i++){cmde[i]=' ';}

	pfp.code = 3;

	cout << "Host name " << s << endl;

	// Signal
	signal(SIGINT, (sig_t)bye);

	/* CREATION DES THREADS	*/
	pthread_attr_t *thread_attributes;
	pthread_t *thread;

	/* creation du thread processus communication au sol */
	tid_sol = 1;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, &Communic_Sol,(void *) NULL) != 0)
		perror ("Thread_Server-> Failure detector thread pb!");

	/* creation du thread processus communication interne */
	tid_interne=2;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, &Communic_Interne, (void *) argv_void) != 0)  
		perror ("Thread_Server-> Image mgt thread pb!");

	while (1) {usleep(100);}  /* DOES NOTHING */ ;

	return 0;
}
