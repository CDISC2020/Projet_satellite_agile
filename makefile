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

ARINC_Com:  $(ARINC)ARINC_Com.cpp $(ARINC)ARINC_Com.h
	$(CC) -c $(ARINC)ARINC_Com.cpp $(ARINC)ARINC_Com.h

genericInstruction: $(PLAN)genericInstruction.cpp $(PLAN)genericInstruction.h
	$(CC) -c $(PLAN)genericInstruction.cpp $(PLAN)genericInstruction.h

plan: $(PLAN)plan.cpp $(PLAN)plan.h
	$(CC) -c $(PLAN)plan.cpp $(PLAN)plan.h

planManager: $(PLAN)planManager.cpp $(PLAN)planManager.h $(PLAN)main_PM.cpp $(FDIR)FDIR.h
	$(CC) -c $(PLAN)planManager.cpp $(PLAN)planManager.h $(PLAN)main_PM.cpp $(FDIR)FDIR.h

comGroundManager: $(COM)comGroundManager.cpp $(FDIR)FDIR.h
	$(CC) -c $(COM)comGroundManager.cpp $(FDIR)FDIR.h

statusManager:  $(COM)statusManager.cpp $(COM)statusManager.h
	$(CC) -c $(COM)statusManager.cpp $(COM)statusManager.h

ground: $(COM)Ground.cpp
	$(CC) -c $(COM)Ground.cpp

attitudeController:  $(PLAN)attitudeController.cpp $(PLAN)attitudeController.h
	$(CC) -c $(PLAN)attitudeController.cpp $(PLAN)attitudeController.h

cameraController:  $(PLAN)cameraController.cpp $(PLAN)cameraController.h
	$(CC) -c $(PLAN)cameraController.cpp $(PLAN)cameraController.h

controller: $(PLAN)Controller.cpp $(PLAN)Controller.h
	$(CC) -c $(PLAN)Controller.cpp $(PLAN)Controller.h

gpio : $(GPIO)GPIO.cpp $(GPIO)GPIO.h
	$(CC) -c $(GPIO)GPIO.cpp $(GPIO)GPIO.h

fdir: $(FDIR)FDIR.cpp $(FDIR)FDIR.h $(FDIR)watchdog_arduino.cpp $(FDIR)watchdog_arduino.h $(FDIR)WatchdogInterne.cpp $(FDIR)WatchdogInterne.h $(FDIR)main_FDIR.cpp
	$(CC) -c $(FDIR)FDIR.cpp $(FDIR)FDIR.h $(FDIR)watchdog_arduino.cpp $(FDIR)watchdog_arduino.h $(FDIR)WatchdogInterne.cpp $(FDIR)WatchdogInterne.h $(FDIR)main_FDIR.cpp


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
kernel: $(ARINC)kernel_arinc.cpp $(ARINC)time_frame.h $(ARINC)config_kernel.h
	gcc $(ARINC)kernel_arinc.cpp


############################################################################

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.gch" -type f -delete

success:
	echo "\n[ Make successful! ]\n"

reset:
	find . -name "main_PM" -type f -delete
	find . -name "main_Com_ST" -type f -delete
	find . -name "main_FDIR" -type f -delete
	find . -name "main_Ground" -type f -delete
	find . -name "a.out" -type f -delete
	find . -name "listpid" -type f -delete
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
