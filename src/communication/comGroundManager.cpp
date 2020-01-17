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
#include <algorithm>

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
	PlanFilePath pfp;

	string dir = string(".");
    	vector<string> files;
	vector<string>::iterator it;

	ifstream ifile;
	string name;
	while(1)
	{
		if(mode == true)
		{
			files.clear();
    			getdir(dir,files);

			name="demande_imgs.txt";
			it=find(files.begin(),files.end(),name);
			if(it!=files.end()) // Si fichier présent dans la liste
			{
		   		ifile.open(name);
				if(ifile) // si on arrive à l'ouvrir
				{
					cout << "File find" << endl;

					//sprintf(cmde, "sh src/communication/uploadStoG.sh, "LogError.txt");
					//system(cmde);

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

					ifile.close();
					remove(name.c_str());
					cout << "Demande enlevée\n";
				}
			}

			name="plan.txt";
			it=find(files.begin(),files.end(),name);
			if(it!=files.end()) // Si fichier présent dans la liste
			{
		   		ifile.open(name);
				if(ifile) // si on arrive à l'ouvrir
				{
					// Faire des trucs

					// Adapter ici le path du fichier pour l'executer dans la suite
					// (apres l'avoir déplacé)
					// if y a un plan
					/*for (unsigned int i = 0;i < files.size();i++)
						pfp.filepath[i] = files[i][0];
					cout << pfp.filepath << "buffer pfp";
					channelOutPM.SendQueuingMsg((char*)&pfp, sizeof(PlanFilePath));*/

					ifile.close();
					//remove(name.c_str()); // deplace dans src/planMagager/planRecu avec system()
				}
			}

			else
				cout << "No file" << endl;
		}

		else
			cout << "In follower mode" << endl;

		usleep(100);
	}

	return NULL;
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

		// changement mode primary ou backup
		else if(status->code == 6)
		{
			m = (ModeStruct*)buffer;
			mode = m->rpiMode;
		}
	}

	return NULL;
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
	// Check si argument fourni
	if (argc!=2)
	{
		printf("T'as oublie l'argument pinpin ! Le hostname... \n");
		exit (-1);
	}

	// Recuperation du hostname
	if (gethostname(s, 100) != 0)
	{
		perror("S-> gethostname");
		exit(1);
	}
	cout << "Host name " << s << endl;

	// Cast des arguments pour les envoyer aux threads
	void *argv_void = static_cast<void*>(argv[1]);

	// Init des tableaux
	for(int i=0; i<1024; i++) buffer[i]='\0';
	for(int i=0; i<50; i++) cmde[i]=' ';

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
