#!/bin/sh
.SUFFIXES : .c .o

# CONSTANTES
CC = g++ -Wall -std=c++11 -lpthread -pthread
MAKE = make

ARINC = src/ARINC/
# Workpackage directories
WP_PLAN = src/planManager/
WP_COM = src/communication/
WS_FDIR = src/FDIR/

# exemples sur les sockets :  quels sockets???
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
	$(MAKE) main_FDIR
	$(MAKE) main_PM
	$(MAKE) main_com
	$(MAKE) main_FDIR
	$(MAKE) main_Ground
	$(MAKE) kernel
	$(MAKE) clean
	$(MAKE) success

ARINC_Com:  $(ARINC)ARINC_Com.cpp $(ARINC)ARINC_Com.h
	$(CC) -c $(ARINC)ARINC_Com.cpp $(ARINC)ARINC_Com.h

genericInstruction: $(WP_PLAN)genericInstruction.cpp $(WP_PLAN)genericInstruction.h
	$(CC) -c $(WP_PLAN)genericInstruction.cpp $(WP_PLAN)genericInstruction.h

plan: $(WP_PLAN)plan.cpp $(WP_PLAN)plan.h
	$(CC) -c $(WP_PLAN)plan.cpp $(WP_PLAN)plan.h

planManager: $(WP_PLAN)planManager.cpp $(WP_PLAN)planManager.h $(WP_PLAN)main_PM.cpp
	$(CC) -c $(WP_PLAN)planManager.cpp $(WP_PLAN)planManager.h $(WP_PLAN)main_PM.cpp

comGroundManager: $(WP_COM)comGroundManager.cpp $(WS_FDIR)FDIR.h
	$(CC) -c $(WS_FDIR)FDIR.h $(WP_COM)comGroundManager.cpp

statusManager:  $(WP_COM)statusManager.cpp $(WP_COM)statusManager.h
	$(CC) -c $(WP_COM)statusManager.cpp $(WP_COM)statusManager.h
	$(CC) -c $(WP_COM)statusManager.cpp

ground: $(WP_COM)Ground.cpp
	$(CC) -c $(WP_COM)Ground.cpp

attitudeController:  $(WP_PLAN)attitudeController.cpp $(WP_PLAN)attitudeController.h
	$(CC) -c $(WP_PLAN)attitudeController.cpp $(WP_PLAN)attitudeController.h

cameraController:  $(WP_PLAN)cameraController.cpp $(WP_PLAN)cameraController.h
	$(CC) -c $(WP_PLAN)cameraController.cpp $(WP_PLAN)cameraController.h

controller: $(WP_PLAN)Controller.cpp $(WP_PLAN)Controller.h
	$(CC) -c $(WP_PLAN)Controller.cpp $(WP_PLAN)Controller.h

gpio : src/GPIO.cpp src/GPIO.h
	$(CC) -c src/GPIO.cpp

main_PM: ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o main_PM.o Controller.o $(WS_FDIR)FDIR.h
	$(CC) ARINC_Com.o genericInstruction.o $(WS_FDIR)FDIR.h plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o main_PM.o Controller.o -o main_PM

main_com: ARINC_Com.o statusManager.o comGroundManager.o
	$(CC) ARINC_Com.o statusManager.o comGroundManager.o -o main_Com_ST

main_FDIR: $(WS_FDIR)main_FDIR.cpp 
	$(CC) $(WS_FDIR)main_FDIR.cpp $(WS_FDIR)FDIR.cpp $(ARINC)ARINC_Com.cpp 	$(WS_FDIR)WatchdogInterne.cpp $(WS_FDIR)watchdog_arduino.cpp src/GPIO.cpp  -o main_FDIR

main_Ground: ARINC_Com.o Ground.o
	$(CC) ARINC_Com.o Ground.o -o main_Ground

kernel: $(ARINC)kernel_arinc.cpp $(ARINC)time_frame.h $(ARINC)config_kernel.h
	gcc $(ARINC)kernel_arinc.cpp

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
