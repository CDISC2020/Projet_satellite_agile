#!/bin/sh
.SUFFIXES : .c .o

# CONSTANTES
CC = g++ -Wall -std=c++11

THREAD = -lpthread -pthread
MAKE = make

# ARINC directory
ARINC = src/ARINC/

# GPIO directory
GPIO = src/

# Ground directory
GROUND = Ground/

# Partition directories
PLAN = src/planManager/
COM = src/communication/
FDIR = src/FDIR/

all:
	$(MAKE) clean
	$(MAKE) ARINC_Com
	$(MAKE) genericInstruction
	$(MAKE) plan
	$(MAKE) planManager
	$(MAKE) comGroundManager
	$(MAKE) statusManager
	$(MAKE) ground
	$(MAKE) attitudeController
	$(MAKE) cameraController
	$(MAKE) controller
	$(MAKE) gpio
	$(MAKE) fdir
	$(MAKE) main_PM
	$(MAKE) main_com
	$(MAKE) main_FDIR
	$(MAKE) main_Ground
	$(MAKE) kernel
	$(MAKE) clean
	$(MAKE) success


############################ COMPILATION ################################

ARINC_Com:  $(ARINC)ARINC_Com.cpp
	$(CC) -c $(ARINC)ARINC_Com.cpp

genericInstruction: $(PLAN)genericInstruction.cpp 
	$(CC) -c $(PLAN)genericInstruction.cpp 

plan: $(PLAN)plan.cpp
	$(CC) -c $(PLAN)plan.cpp

planManager: $(PLAN)planManager.cpp $(PLAN)main_PM.cpp
	$(CC) -c $(PLAN)planManager.cpp $(PLAN)main_PM.cpp

comGroundManager: $(COM)comGroundManager.cpp 
	$(CC) -c $(COM)comGroundManager.cpp 

statusManager:  $(COM)statusManager.cpp 
	$(CC) -c $(COM)statusManager.cpp

ground: $(COM)Ground.cpp
	$(CC) -c $(COM)Ground.cpp

attitudeController:  $(PLAN)attitudeController.cpp
	$(CC) -c $(PLAN)attitudeController.cpp

cameraController:  $(PLAN)cameraController.cpp
	$(CC) -c $(PLAN)cameraController.cpp

controller: $(PLAN)Controller.cpp 
	$(CC) -c $(PLAN)Controller.cpp

gpio : $(GPIO)GPIO.cpp
	$(CC) -c $(GPIO)GPIO.cpp

fdir: $(FDIR)FDIR.cpp $(FDIR)watchdog_arduino.cpp $(FDIR)WatchdogInterne.cpp $(FDIR)main_FDIR.cpp
	$(CC) -c $(FDIR)FDIR.cpp $(FDIR)watchdog_arduino.cpp $(FDIR)WatchdogInterne.cpp $(FDIR)main_FDIR.cpp


############################### LINKAGE ####################################

##############
# Partitions #
##############
main_PM: ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o main_PM.o Controller.o 
	$(CC) $(THREAD) ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o main_PM.o Controller.o -o main_PM

main_com: ARINC_Com.o statusManager.o comGroundManager.o
	$(CC) $(THREAD) ARINC_Com.o statusManager.o comGroundManager.o -o main_Com_ST

main_FDIR: ARINC_Com.o main_FDIR.o FDIR.o WatchdogInterne.o watchdog_arduino.o GPIO.o
	$(CC) $(THREAD) ARINC_Com.o main_FDIR.o FDIR.o WatchdogInterne.o watchdog_arduino.o GPIO.o -o main_FDIR

######################
# Station Sol Ground #
######################
main_Ground: Ground.o
	$(CC) Ground.o -o $(GROUND)main_Ground

###############
# Noyau ARINC #
###############
kernel: $(ARINC)kernel_arinc.c $(ARINC)time_frame.h $(ARINC)config_kernel.h
	gcc $(ARINC)kernel_arinc.c


############################################################################

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.gch" -type f -delete
success:
	echo "\n[ Make successful! ]\n"

reset:
	find . -name "*.o" -type f -delete
	find . -name "*.gch" -type f -delete
	find . -name "main_PM" -type f -delete
	find . -name "main_Com_ST" -type f -delete
	find . -name "main_FDIR" -type f -delete
	find . -name "main_Ground" -type f -delete
	find . -name "a.out" -type f -delete
	find . -name "listpid" -type f -delete
	find . -name "pid_COM" -type f -delete
	find . -name "pid_FDIR" -type f -delete
	find . -name "pid_PM" -type f -delete
	find . -name "pid_ARINC" -type f -delete
	find . -name "LogError.txt" -type f -delete
	find src/planManager/plans/ -name "[!.]*" -type f -delete
	find src/planManager/tm/ -name "[!.]*" -type f -delete
	find src/communication/planRecuSol -name "[!.]*" -type f -delete
	find src/communication/tmRecuSol -name "[!.]*" -type f -delete
 
touch:
	touch makefile
	touch src/communication/*
	touch src/planManager/*
	touch src/FDIR/*
	touch src/ARINC/*
