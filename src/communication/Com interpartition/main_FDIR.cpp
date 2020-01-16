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
 
int  main (int argc,char* argv[]) 
{
	cout << "bonjour je suis le main_FDIR" << endl;

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
		printf("Message : %s",buffer);


		if (myFDIR.isleader()==false) 
		{
			if (myFDIR.read_arduino()) 
			{
				printf("AH, YES, ENSLAVED ARDUINO");
			}
			else 
			{
				printf("J'AI DUPER LE SIGNAL");
			}
		}


		if (buffer[0]=='C')
		{
			myFDIR.set_watch_com(); //On remet le compteur à 100
		}
		else
		{
			if (myFDIR.read_watch_com()==0) // on test si le compteur est a 0
			{
				printf("com KC");//ça fait trop longtemps qu'on a pas de nouvelles
			}
			else
			{
				myFDIR.dec_watch_com();// si on a pas de nouvelles, on decremente le compteur
			}
		}
		
		if (buffer[0]=='P')
		{
			myFDIR.set_watch_plan();
		}
		else
		{
			if (myFDIR.read_watch_plan()==0) // on test si le compteur est a 0
			{
				printf("plan KC");//ça fait trop longtemps qu'on a pas de nouvelles
			}
			else
			{
				myFDIR.dec_watch_plan();// si on a pas de nouvelles, on decremente le compteur
			}
		}

	
		if(myFDIR.isleader())
		{
			myFDIR.set_arduino();
		}
	

	}

	return 0;
}
