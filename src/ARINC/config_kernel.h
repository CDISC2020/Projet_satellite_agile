// Be careful ! //
// In kernel_arinc.c,
//
// Initialisation of P_in_frame and Tframe
// 	P_in_frame,Tframe [length=A]
//
//Reading partition ids (pids)
// 	pid [length=N]

// if modified, adapt kernel_arinc.c acording to the note above
#define N 3  // number of partitions
#define A 3  // number of partition activation per time frame 

// Time budget for partitions ACTIVATION in milliseconds
#define SFDIR1 5
#define SCOM1 10
#define SPM1 10


