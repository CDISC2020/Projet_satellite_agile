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
char s[100];
 
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
	if (gethostname(s, 100) != 0) 
	{
    		perror("S-> gethostname");
    		exit(1);
  	}
	cout << "Host name " << s << endl; 

	QueuingPort channel(1, 18002, s); // Server
	sleep(500000);
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

	QueuingPort channelPM(0, 18001, argv[1]);
        channelPM.SendQueuingMsg((char*)&mode, sizeof(ModeStruct));

	QueuingPort channelCom(0, 18003, argv[1]);
        channelCom.SendQueuingMsg((char*)&mode, sizeof(ModeStruct));


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
				cout << "com KC" << endl; //ça fait trop longtemps qu'on a pas de nouvelles
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
				cout << "plan KC" << endl; //ça fait trop longtemps qu'on a pas de nouvelles
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
