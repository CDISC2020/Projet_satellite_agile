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
#include "time_frame.h"

pid_t pid[N]; // table of pid, one per partition 

int Tperiod[N]; // table of periods, i.e. time budget per partition

int Tframe[A]; // table of time slot allocated to partition in the major frame
int P_in_frame[A]; // table of partition actuvation sequence

int status;

int i;

pid_t p;

int active_p; // id of the running process

// main control loop

int main(int argc, char *argv[]) {

if ((argc==0) || (argc==1) ) {
	printf("Ooops! T'as oublié les arguments, les pid des process! (argc=%d)\n", argc);
	exit(1);
}

printf("*** TRACE : %d %s %s %s %s %s %s \n", argc, argv[0], argv[1],argv[2],argv[3],argv[4],argv[5]);

// Initialisation of the periods
Tperiod[0]=TIME_P1;
Tperiod[1]=TIME_P2;
Tperiod[2]=TIME_P3;

// Initialisation of P_in_frame and Tframe
P_in_frame[0]=0; 	Tframe[0]=SFDIR1;
P_in_frame[1]=1; 	Tframe[1]=SCOM1;
P_in_frame[2]=0; 	Tframe[2]=SFDIR2;
P_in_frame[3]=2;	Tframe[3]=SPM1;


// Main body of the program

// Reading partition ids (pids)
	pid[0] = atoi(argv[1]) ; // FIRST process-partition P1
	pid[1] = atoi(argv[2]) ; // SECOND process-partition P2
	pid[2] = atoi(argv[3]) ; // THIRD process-partition P3

printf("********* LIST OF PARTITION IDS************\n");

for (i=0; i<N; i++)
	printf("pids - P%d:	%d\n",i,pid[i]);

printf("********* LIST OF PARTITION TIME BUDGET ************\n");

for (i=0; i<N; i++)
	printf("TIME_P - P%d:	%d\n",i,Tperiod[i]);

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

sleep (10);



printf("********* STARTING SCHEDULING LOOP ************\n");


active_p=0; // active process active_p set to 0 (1st process in major frame)

while (1) {  // Scheduler infinite loop

	// next process to schedule
	printf("Active P(%d) – PID: %d – ", active_p, pid[P_in_frame[active_p]]);
	kill (pid[P_in_frame [active_p]], SIGCONT);

	// scheduler waiting for active_p time budget
	//printf("Tperiod: %d ms\n", Tperiod[active_p]);
	printf("T: %d ms\n", Tframe[active_p]);
	//usleep (Tperiod[active_p]*1000); // parameter in usec
	usleep (Tframe[active_p]*1000); // parameter in usec

	// stop active_p after time budget elapsed
	kill (pid[P_in_frame[active_p]], SIGSTOP);

	// set active_p to next (modulo N, number of partitions)
	
	active_p=(active_p + 1) % A;
	
} // end of while - end of main loop

}; // end of program
