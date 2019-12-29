#!/bin/sh
.SUFFIXES : .c .o 

# CONSTANTES 
CC = g++ -Wall -std=c++11 -lpthread -pthread
MAKE = make

ARINC = src/ARINC/
# Workpackage directories
WP_PLAN = src/planManager/
WP_COM = src/communication/
WP_SAF = src/safety/

# exemples sur les sockets :  quels sockets???
all:
	$(MAKE) ARINC_Com
	$(MAKE) genericInstruction
	$(MAKE) plan
	$(MAKE) planManager
	$(MAKE) comGroundManager
	$(MAKE) statusManager
	$(MAKE) ground
	$(MAKE) attitudeController
	$(MAKE) cameraController
	$(MAKE) control
	$(MAKE) gpio
	$(MAKE) watchdog
	$(MAKE) main_PM 
	$(MAKE) main_run
	$(MAKE) main_com
	$(MAKE) main_Ground
	$(MAKE) main_control
	$(MAKE) kernel
	$(MAKE) clean
	$(MAKE) success

ARINC_Com:  $(ARINC)ARINC_Com.cpp $(ARINC)ARINC_Com.h
	$(CC) -c $(ARINC)ARINC_Com.cpp

genericInstruction: $(WP_PLAN)genericInstruction.cpp $(WP_PLAN)genericInstruction.h
	$(CC) -c $(WP_PLAN)genericInstruction.cpp

plan: $(WP_PLAN)plan.cpp $(WP_PLAN)plan.h
	$(CC) -c $(WP_PLAN)plan.cpp

planManager: $(WP_PLAN)planManager.cpp $(WP_PLAN)planManager.h
	$(CC) -c $(WP_PLAN)planManager.cpp 

comGroundManager: $(WP_COM)comGroundManager.cpp
	$(CC) -c $(WP_COM)comGroundManager.cpp

statusManager:  $(WP_COM)statusManager.cpp $(WP_COM)statusManager.h
	$(CC) -c $(WP_COM)statusManager.cpp

ground: $(WP_COM)Ground.cpp
	$(CC) -c $(WP_COM)Ground.cpp

attitudeController:  $(WP_PLAN)attitudeController.cpp $(WP_PLAN)attitudeController.h
	$(CC) -c $(WP_PLAN)attitudeController.cpp

cameraController:  $(WP_PLAN)cameraController.cpp $(WP_PLAN)cameraController.h
	$(CC) -c $(WP_PLAN)cameraController.cpp

control: $(WP_PLAN)control.cpp
	$(CC) -c $(WP_PLAN)control.cpp
	
gpio : src/GPIO.cpp src/GPIO.h
	$(CC) -c src/GPIO.cpp
	
watchdog : $(WP_SAF)watchdog.cpp $(WP_SAF)watchdog.h
	$(CC) -c $(WP_SAF)watchdog.cpp

main_PM: $(WP_PLAN)main_PM.cpp
	$(CC) -c $(WP_PLAN)main_PM.cpp

main_run: ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o watchdog.o main_PM.o
	$(CC) ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o watchdog.o main_PM.o -o main_run

main_com: ARINC_Com.o statusManager.o comGroundManager.o
	$(CC) ARINC_Com.o statusManager.o comGroundManager.o -o main_Com_ST

main_Ground: ARINC_Com.o Ground.o
	$(CC) ARINC_Com.o Ground.o -o main_Ground

main_control: ARINC_Com.o attitudeController.o cameraController.o control.o
	$(CC) ARINC_Com.o attitudeController.o cameraController.o control.o -o main_Control

kernel: $(ARINC)kernel_arinc.cpp $(ARINC)time_frame.h $(ARINC)config_kernel.h
	gcc $(ARINC)kernel_arinc.cpp

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.gch" -type f -delete

success:
	echo "\n[ Make successful! ]\n"

reset:
	find . -name "main_Com_ST" -type f -delete
	find . -name "main_Ground" -type f -delete
	find . -name "main_Control" -type f -delete
	find . -name "main_run" -type f -delete
	find . -name "a.out" -type f -delete
	find . -name "listpid" -type f -delete
	find . -name "LogError.txt" -type f -delete

