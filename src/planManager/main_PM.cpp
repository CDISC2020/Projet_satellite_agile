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

#include "planManager.h"
#include "Controller.h"
#include "../communication/statusManager.h"
#include "../GPIO.h"
#define LOW 0

using namespace std;


int tid_FD, tid_FP, tid_A;
pthread_t *thread1, *thread2;

PlanManager PM;
Controller control;

char s[100];

bool mode=false; // 'false' pour follower et 'true' pour leader // Toujours init a false

QueuingPort* channelFDIR;		// Client vers FDIR
QueuingPort* channelSM; 		// Client_PM vers com ground / SM
QueuingPort* channelReceptionPM; 	// Server_PM

pthread_mutex_t* mu=new pthread_mutex_t();  // Mutex sur la gestion des plans

/*-----------------------SERVEUR--------------------------*/
void * Server_PM(void *args)
{
	int x=0;

	PlanFilePath* f;
	Status* s;
	ModeStruct* m;

	char buffer[1024];
	int i; for(i=0; i>1024; i++) buffer[i] = '\0';

	while(1)
	{
		channelReceptionPM->RecvQueuingMsg(buffer);
		s = (Status*)buffer;

		if(s->code == 3) // nouveau plan
		{
			// ARRIVE de COM
			f=(PlanFilePath*)buffer;
			cout<<"Msg("<<x++<<"):  path ="<<f->filepath<<endl;


			pthread_mutex_lock(mu);              // verrouiller la ressource partagé
			PM.generatePlan(f->filepath);
			pthread_mutex_unlock(mu);            // deverrouiller la ressource partagé
		}

		else if(s->code == 6) // Changement de mode
		{
			// ARRIVE de FDIR
			m = (ModeStruct*)buffer;
			mode = m->rpiMode;
		}

	        else if (s->code == 17) // telecommande
                {
			// ARRIVE de COM 
			f=(PlanFilePath*)buffer;
			cout<<"Msg("<<x++<<"):  path ="<<f->filepath<<endl;

			pthread_mutex_lock(mu);              // verrouiller la ressource partagé
			Plan* planBack = PM.backup();
			// Si on a bien un plan actif
			if(planBack!=NULL)	// On charge la telecommande
				PM.destructPlan();
			PM.generatePlan(f->filepath); 

			int N =PM.getNInstru(); // On execute la telecommande
			for(int i=0; i<N; i++)
				PM.executePlan(&control, channelSM, mode);

			PM.destructPlan();	// On recharge le plan precedent
			if(planBack!=NULL)
				PM.recover(planBack);
			pthread_mutex_unlock(mu);            // deverrouiller la ressource partagé
                }

		usleep(100);
	}

	cout << "Terminaison du Thread " << tid_FD << endl;
	return NULL;
}


/*--------------------FONCTIONNEMENT----------------------*/
void before()
{
// Maintenant c'est le FDIR qui gère les watchdog, donc plus ici ! :)
}

void proceed()
{
	// Fonctionnement normal
	cout << endl << "Fonctionnement" << endl;
	pthread_mutex_lock(mu);              // verrouiller la ressource partagé
	PM.executePlan(&control, channelSM, mode);
	pthread_mutex_unlock(mu);            // deverrouiller la ressource partagé
	usleep(500000);
}

void after()
{
	// rien
}

void * Client_PM(void *args)
{
	while(1)
	{
		// probleme watchdog periode d'execution/periode WD marche pas, autre thread ?
		before();
		proceed();
		after();

		usleep(5*1000); // 5 ms
	}

	return NULL;
}


/*-----------------------ALIVE----------------------------*/
void* Alive(void* args)
{
	while(1)
	{	
		//str[100]='C' si vous êtes sur la Com, ='P' si vous êtes sur le plan
		char str[2]="P";
		channelFDIR->SendQueuingMsg(str, sizeof(str));

		usleep(10*1000); //10 ms
	}

	return NULL;
}


/*------------------------SIGNAL--------------------------*/
sig_t bye()
{
	cout << "Mort........" << endl;
	exit(0);
}

/*-------------------------MAIN---------------------------*/
int  main (int argc,char* argv[])
{
	// Recuperation du hostname
	if (gethostname(s, 100) != 0)
	{
	    perror("S-> gethostname");
	    exit(1);
	}
	cout << "Host name " << s << endl;

	signal(SIGINT, (sig_t)bye);

	channelFDIR = new QueuingPort(0, 18002, s);		// Client FDIR
	channelSM = new QueuingPort(0, 18003, s); 		// Client COM
	channelReceptionPM = new QueuingPort(1, 18001, s); 	// Server

	usleep(500000);

	channelFDIR->Display();
	channelSM->Display();
	channelReceptionPM->Display();

	// generate thread
	pthread_attr_t *thread_attributes;
	pthread_t *thread;

	/* creation du thread Server1 */
	tid_FD = 1;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	thread1 = thread;
	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, Server_PM,(void *) NULL) != 0)
		perror ("Thread_Server-> Failure detector thread pb!");

	/* creation du thread Client1 */
	tid_FP = 2;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	thread2 = thread;
	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, Client_PM,(void *) NULL) != 0)
		perror ("Thread_Client-> Failure detector thread pb!");

	/* creation du thread Client2 */
	tid_A = 3;

	thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
	thread=(pthread_t *)malloc(sizeof(pthread_t));

	thread2 = thread;
	pthread_attr_init(thread_attributes);
	if (pthread_create(thread, thread_attributes, Alive,(void *) NULL) != 0)
		perror ("Thread_Alive-> Failure detector thread pb!");

	while(1) {usleep(100);}

	return 0;
}
