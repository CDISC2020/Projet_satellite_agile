#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "config_kernel.h"

pid_t pid[N]; // table of pid, one per partition 

int Tframe[A]; // table of time slot allocated to partition in the major frame
int P_in_frame[A]; // table of partition actuvation sequence

int status;

int i;

pid_t p;

int active_p; // id of the running process

void update_pids(int sig)
{
	FILE* file=NULL;
	file=fopen("listpid", "r");
	if(!file)
		printf("Failed to read listpid");
	else
	{
		int tab[N] = {0};
		fscanf(file,"%d\n%d\n%d\n", pid, pid+1, pid+2); // En fonction de N
		printf("New pids:\n%d\n%d\n%d\n", pid[0], pid[1], pid[2]); // fct N
	}
}

// main control loop
int main(int argc, char *argv[]) 
{
	if ((argc==0) || (argc==1) ) {
		printf("Ooops! T'as oublié les arguments, les pid des process! (argc=%d)\n", argc);
		exit(1);
	}
	
	signal(SIGUSR1, update_pids);

	printf("*** TRACE : %d %s %s %s %s %s %s \n", argc, argv[0], argv[1],argv[2],argv[3],argv[4],argv[5]);

	// Initialisation of P_in_frame and Tframe (A)
	P_in_frame[0]=0; 	Tframe[0]=SFDIR1;
	P_in_frame[1]=1; 	Tframe[1]=SCOM1;
	P_in_frame[2]=2;	Tframe[2]=SPM1;

	// Reading partition ids (pids) (N)
		pid[0] = atoi(argv[1]) ; // FIRST process-partition P1
		pid[1] = atoi(argv[2]) ; // SECOND process-partition P2
		pid[2] = atoi(argv[3]) ; // THIRD process-partition P3


	// Writing ARINC pid in a file
	int pid_ARINC=getpid();
	FILE* fichierPid=NULL;
	fichierPid=fopen("pid_ARINC","w");
	if(!fichierPid)
	{
		printf("Failed to open pid_ARINC");
		exit(1);
	}
	fprintf(fichierPid,"%d\n",pid_ARINC);
	fclose(fichierPid);
	kill(pid[0], SIGUSR1); // telling FDIR it's done so it can read it



	// Main body of the program
	printf("********* LIST OF PARTITION IDS************\n");

	for (i=0; i<N; i++)
		printf("pids - P%d:	%d\n",i,pid[i]);

	printf("********* LIST OF PARTITION IN THE TIME FRAME  ************\n");

	for (i=0; i<A; i++)
		printf("P%d:	%d\n",P_in_frame[i],Tframe[i]);

	printf("********* STOPPING ALL PARTITION IDS************\n");

	for (i=0; i<N; i++){
		printf("STOP - P%d:     %d\n",i,pid[i]);
		kill (pid[i],  SIGSTOP);
	}

	printf("********* WAITING BEF0RE SCHEDULING LOOP  ************\n");
	printf("********* All partitions must be stopped  ************\n");

	sleep (5);



	printf("********* STARTING SCHEDULING LOOP ************\n");


	active_p=0; // active process active_p set to 0 (1st process in major frame)

	while (1) {  // Scheduler infinite loop

		// next process to schedule
		printf("Active P(%d) – PID: %d – ", active_p, pid[P_in_frame[active_p]]);
		kill (pid[P_in_frame [active_p]], SIGCONT);

		// scheduler waiting for active_p time budget
		printf("T: %d ms\n", Tframe[active_p]);
		usleep (Tframe[active_p]*1000); // parameter in usec

		// stop active_p after time budget elapsed
		kill (pid[P_in_frame[active_p]], SIGSTOP);

		// set active_p to next (modulo A, number of partitions per time frame)
		active_p=(active_p + 1) % A;
		
	} // end of while - end of main loop

}; // end of program
