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
	char cmde[] = {"                                "};
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
		cout << "('p' to send a plan ; 'r' to receive the photos)" << endl;
		char c;
		scanf(" %c", &c);

		if (c == 'p')	// Envoi d'un plan
		{
			cout << "What is the plan name ? (end it by '.txt'" << endl;
			p.code = 5;
			scanf("%11s[^\n]", p.name);
			cout << "Sending Plan..." << endl;	
			sprintf(cmde, "sh src/communicaton/uploadGtoS.sh %s", p.name);
			system(cmde);
			sleep(2);

			cout << "Plan sent !" << endl;
		}
		else if (c=='r')  // Demande des photos
		{
			p.code = 10;
			//char file_demande = "demande_imgs.txt";
			sprintf(cmde, "sh src/communication/uploadGtoS.sh %s", "demande_imgs.txt");
			printf("demande image envoyee");
			system(cmde);
			cout << "Receiving the images..." << endl;
			sleep(2);
		}
	}

	return 0;
}
