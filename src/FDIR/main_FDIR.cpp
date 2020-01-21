#include <iostream>
#include <string>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include "FDIR.h" 

using namespace std;

#include "../ARINC/ARINC_Com.h"

FDIR myFDIR;
bool set_wd_ard=false;
char s[100];
bool rec_COM=false;
bool rec_PM=false;

int arinc_pid=-1;
 
void* gestion_wd_arduino(void* args)
{
	while(1)
	{
		if(set_wd_ard==true)
		{
			myFDIR.set_arduino();//on demande a mettre la pin a 1
			usleep(5000);
			myFDIR.reset_arduino();//on demande a mettre la pin a 0
			usleep(20000);
			set_wd_ard=false;
		}

		usleep(100);
	}

	return NULL;
}

void maj_listpid()
{
	string line;
	string listpid;
	ifstream fichier;
	ofstream wfichier;

	// Lecture pid FDIR
	fichier.open("pid_FDIR");
	if(!fichier){
		cout << "Failed to open pid_FDIR" << endl;
		exit(1);
	}
	getline(fichier,line);
	listpid.append(line);
	listpid.append("\n");
	fichier.close();

	// Lecture pid COM
	fichier.open("pid_COM");
	if(!fichier){
		cout << "Failed to open pid_FDIR" << endl;
		exit(1);
	}
	getline(fichier,line);
	listpid.append(line);
	listpid.append("\n");
	fichier.close();

	// Lecture pid PM
	fichier.open("pid_PM");
	if(!fichier){
		cout << "Failed to open pid_FDIR" << endl;
		exit(1);
	}
	getline(fichier,line);
	listpid.append(line);
	listpid.append("\n");
	fichier.close();

////////// Ecritude du listpid
	cout << "listpid=" << listpid << endl;
	wfichier.open("listpid");
	if(!wfichier)
	{
		cout << "Failed to open listpid" << endl;
		exit(1);
	}
	wfichier << listpid;
	wfichier.close();
}	

void recouvrement_COM()
{
	// tolere faute par crash pas par freeze, sinon faut tuer process avant et donc recup les pid de tout le monde

	if(rec_COM)
	{
		// autre time out
		cout << "faut changer de raspy my boi" << endl;
	}
	else
	{
		rec_COM=true;

		raise(SIGUSR2);

		/*// reboot COM
		system("xterm -e ./main_COM &");

		// maj du listpid
		maj_listpid();

		// previent ARINC qu'il faut prendre en compte la modif de listpid
		if(arinc_pid!=-1)
			kill(arinc_pid,SIGUSR1);*/
	}
}

void recouvrement_PM()
{
	// tolere faute par crash pas par freeze, sinon faut tuer process avant et donc recup les pid de tout le monde

	if(rec_PM)
	{
		cout << "faut changer de raspy my boi" << endl;
	}
	else
	{
		rec_PM=true;
		raise(SIGUSR2);

		/*// reboot PM
		system("xterm -e ./main_PM &");

		// maj du listpid
		maj_listpid();

		// previent ARINC qu'il faut prendre en compte la modif de listpid
		if(arinc_pid!=-1)
			kill(arinc_pid,SIGUSR1);*/
	}
}

// signal handler
void ARINC_PID(int signal)
{
	ifstream fichier;
	fichier.open("pid_ARINC");
	string line;
	if(!fichier)
	{
		cout << "Failed to open pid_ARINC" << endl;
		exit(1);
	}
	getline(fichier,line);
	arinc_pid=atoi(line.c_str());
}

void signal2(int sig)
{
	cout << "MAJ PID" << endl;
	system("xterm -e ./main_PM &");
	sleep(1);
	maj_listpid();
	cout << "arinc_pid=" << arinc_pid << endl;
	if(arinc_pid!=-1)
		kill(arinc_pid,SIGUSR1);
}

int  main (int argc,char* argv[]) 
{
	signal(SIGUSR1,ARINC_PID);
	signal(SIGUSR2,signal2);

	// Ecriture du pid dans un fichier
	int pid=getpid();
	ofstream fichierPid;
	fichierPid.open("pid_FDIR");
	if(!fichierPid)	{
		cout << "Failed to open pid_FDIR" << endl;
		exit(1);
	}
	fichierPid << pid << endl;
	fichierPid.close();

	
	if (gethostname(s, 100) != 0) {
    		perror("S-> gethostname");
    		exit(1);
  	}
	cout << "Host name " << s << endl; 

	QueuingPort channel(1, 18002, s); // Server
	channel.Display();	

        pthread_attr_t *thread_attributes;
        pthread_t *thread;

        thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
        thread=(pthread_t *)malloc(sizeof(pthread_t));

        pthread_attr_init(thread_attributes);
        if (pthread_create(thread, thread_attributes, gestion_wd_arduino,(void *) NULL) != 0)
                perror ("Thread_Server-> Failure detector thread pb!");

	sleep(2);

	ModeStruct mode;
	mode.code=6;
	mode.rpiMode=myFDIR.isleader();

	QueuingPort channelPM(0, 18001, s);
        channelPM.SendQueuingMsg((char*)&mode, sizeof(ModeStruct));

	QueuingPort channelCom(0, 18003, s);
        channelCom.SendQueuingMsg((char*)&mode, sizeof(ModeStruct));


	char buffer[100];
	
	while(1) 
	{	
		channel.RecvQueuingMsg(buffer);
		//printf("Message : %s \n",buffer);

		if (buffer[0]=='C')
		{
			myFDIR.set_watch_com(); //On remet le compteur à 100
			set_wd_ard=true;
		}
		else
		{
			if (myFDIR.read_watch_com()==0) // on test si le compteur est a 0
			{
				cout << "com KC" << endl; //ça fait trop longtemps qu'on a pas de nouvelles
				recouvrement_COM();
			}
			else
				myFDIR.dec_watch_com();// si on a pas de nouvelles, on decremente le compteur
		}
		
		if (buffer[0]=='P')
		{
			myFDIR.set_watch_plan();
			set_wd_ard=true;
		}
		else
		{
			if (myFDIR.read_watch_plan()==0) // on test si le compteur est a 0
			{
				cout << "plan KC" << endl; //ça fait trop longtemps qu'on a pas de nouvelles
				recouvrement_PM();
			}
			else
				myFDIR.dec_watch_plan();// si on a pas de nouvelles, on decremente le compteur
		}
		
		if(!myFDIR.isleader())
		{
			if (myFDIR.read_arduino()) 
				cout << "AH, YES, JE VOIS L'ARDUINO " << endl;
			else 			
				cout << "J'AI DUPER LE SIGNAL" << endl;
		}
	}

	return 0;
}
