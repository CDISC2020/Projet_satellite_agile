#include <iostream>
#include <string>
#include <unistd.h>
using namespace std;

#include "../ARINC/ARINC_Com.h"


char buffer[1024];
int main (int argc,char* argv[]) 
{
cout<<"Je suis le PF2, respo plan"<<endl;

if (argc!=2) 
{
	printf("T'as oublié l'argument banane!le hostname... \n");
	exit (-1);
}

// Suite si hostname en argument

cout << "Host name " << argv[1] << endl; 

QueuingPort channelIn(1, 18001, argv[1]); //port plan manager


channelIn.Display();


while (1) {

	channelIn.RecvQueuingMsg(buffer);
	PlanFilePath=(PlanFilePath*)buffer;
	cout << PlanFilePath << "buffer pfp";

}

}
