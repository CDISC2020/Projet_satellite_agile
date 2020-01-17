#include <iostream>
#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

#include "../ARINC/ARINC_Com.h"

int  main ()
{
string msg;

cout<<"Je suis le big boss FDIR, bonjour"<<endl;

int x=0;

char s[100];

if (gethostname(s, 100) != 0) {
    perror("S-> gethostname");
    exit(1);
  }

cout << "Host name " << s << endl; 

QueuingPort channel(1, 18002, s); // Server

channel.Display();

char buffer[1024];

int i; for (i=0; i<1024; i++) buffer[i]='\0';

while(1) {

	channel.RecvQueuingMsg(buffer);
	printf("Msg(%d): %s\n", x++, buffer);
	msg=buffer;
	printf("Attend t'as dit: %s\n", msg);
	}
}
