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

#include "FDIR.h"

using namespace std;

#include "../ARINC/ARINC_Com.h"

FDIR myFDIR;
bool set_wd_ard=false;
 
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
		
	}

	return NULL;
}



int  main (int argc,char* argv[]) 
{
        pthread_attr_t *thread_attributes;
        pthread_t *thread;

        thread_attributes=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
        thread=(pthread_t *)malloc(sizeof(pthread_t));

        pthread_attr_init(thread_attributes);
        if (pthread_create(thread, thread_attributes, gestion_wd_arduino,(void *) NULL) != 0)
                perror ("Thread_Server-> Failure detector thread pb!");


	cout << "bonjour je suis le main_FDIR \n" << endl;

	char s[100];

	if (gethostname(s, 100) != 0) 
	{
    		perror("S-> gethostname");
    		exit(1);
  	}

	cout << "Host name " << s << endl; 

	QueuingPort channel(1, 18002, s); // Server

	channel.Display();

 	

	char buffer[100];
	
	while(1) 
	{	
		channel.RecvQueuingMsg(buffer);
		printf("Message : %s \n",buffer);

		if (buffer[0]=='C')
		{
			myFDIR.set_watch_com(); //On remet le compteur à 100
			set_wd_ard=true;
		}
		else
		{
			if (myFDIR.read_watch_com()==0) // on test si le compteur est a 0
			{
				printf("com KC \n");//ça fait trop longtemps qu'on a pas de nouvelles
			}
			else
			{
				myFDIR.dec_watch_com();// si on a pas de nouvelles, on decremente le compteur
			}
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
				printf("plan KC \n");//ça fait trop longtemps qu'on a pas de nouvelles
			}
			else
			{
				myFDIR.dec_watch_plan();// si on a pas de nouvelles, on decremente le compteur
			}
		}
		
		if(myFDIR.isleader()==false) 
		{
			if (myFDIR.read_arduino()) 
			{
				printf("AH, YES, JE VOIS L'ARDUINO \n");
			}
			else 			
			{
				printf("J'AI DUPER LE SIGNAL \n");
			}
		}
	}

	return 0;
}
