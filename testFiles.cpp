#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dirent.h>


#include <iostream>
using namespace std;


int main()
{
    DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("./TestSatelliteSaturne/")) != NULL) {
		int num = 0;
		char* names_plans;	  	
		while ((ent = readdir (dir)) != NULL) {
		    	printf ("%s\n", ent->d_name);
			names_plans[num] = ent->d_name;
			num ++;
		  	}
		closedir (dir);
		for(int i=0; i<4; i++){
			cout << names_plans[i] << "\n";
		}
	} 
	else {
	  /* could not open directory */
	  perror ("");
	  return EXIT_FAILURE;
	}
}
