#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#include "../planManager/planManager.h"

int main (int argc,char* argv[])
{
	if (argc!=2)
	{
		printf("T'as oublie l'argument banane ! Le hostname... \n");
		exit (-1);
	}

	// Suite si hostname en argument

	char s[100];
	char cmde[100];
	for(int i=0; i<100; i++)
		cmde[i]=' ';
	PlanName p;

	if (gethostname(s, 100) != 0) {
		perror("S-> gethostname");
		exit(1);
	}

	QueuingPort channelOut(0, 18003, argv[1]); 	//Client

	channelOut.Display();

	cout << "Host name " << argv[1] << endl;
	cout << "Welcome to the Ground Station, what do you want to do ?" << endl;

	while (1)
	{
		cout << "What do you want to do ?"<< endl;
		cout << "'p' to send a plan\n" << "'c' to send a commande\n" << "'r' to receive the photos\n" << endl;
		char c;
		scanf(" %c", &c);

		if (c == 'p')	// Envoi d'un plan
		{
			cout << "What is the plan name ? (end it by '.txt')" << endl;
			scanf("%11s[^\n]", p.name);
			cout << "Sending Plan..." << endl;
			sprintf(cmde, "sh src/communication/uploadGtoSplans.sh %s", p.name);
			system(cmde);
			sleep(2);

			cout << "Plan sent !" << endl;
		}

		else if (c=='c')  // Envoi d'une commande
		{
			cout << "What is the commande name ? (end it by '.txt')" << endl;
			scanf("%11s[^\n]", p.name);
			cout << "Sending Commande..." << endl;
			sprintf(cmde, "sh src/communication/uploadGtoStm.sh %s", p.name);
			system(cmde);
			sleep(2);

			cout << "Commande sent !" << endl;
		}

		else if (c=='r')  // Demande des photos
		{
			sprintf(cmde, "sh src/communication/uploadGtoSplans.sh %s", "demande_imgs.txt");
			system(cmde);
			printf("Demande image envoyée");
			cout << "Receiving the images..." << endl;
			sleep(2);
		}
	}

	return 0;
}
