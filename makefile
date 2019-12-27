#!/bin/sh
.SUFFIXES : .c .o 

# CONSTANTES 
CC = g++ -Wall -std=c++11 -lpthread -pthread
MAKE = make


# exemples sur les sockets :  quels sockets???
all:
	$(MAKE) ARINC_Com
	$(MAKE) genericInstruction
	$(MAKE) plan
	$(MAKE) planManager
	$(MAKE) statusManager
	$(MAKE) attitudeController
	$(MAKE) cameraController
	$(MAKE) gpio
	$(MAKE) watchdog
	$(MAKE) main_PM 
	$(MAKE) main_run
	$(MAKE) comGroundManager
	$(MAKE) Ground
	$(MAKE) Control_run
	$(MAKE) kernel
	$(MAKE) clean
	$(MAKE) success

ARINC_Com:  ARINC_Com.cpp ARINC_Com.h
	$(CC) -c ARINC_Com.cpp ARINC_Com.h

genericInstruction: genericInstruction.cpp genericInstruction.h
	$(CC) -c genericInstruction.cpp genericInstruction.h

plan: plan.cpp plan.h
	$(CC) -c plan.cpp plan.h

planManager: planManager.cpp planManager.h
	$(CC) -c planManager.cpp planManager.h

statusManager:  statusManager.cpp statusManager.h
	$(CC) -c statusManager.cpp statusManager.h

attitudeController:  attitudeController.cpp attitudeController.h
	$(CC) -c attitudeController.cpp attitudeController.h

cameraController:  cameraController.cpp cameraController.h
	$(CC) -c cameraController.cpp cameraController.h
	
gpio : GPIO.cpp GPIO.h
	$(CC) -c GPIO.cpp GPIO.h
	
watchdog : watchdog.cpp watchdog.h
	$(CC) -c watchdog.cpp watchdog.h

main_PM: main_PM.cpp
	$(CC) -c main_PM.cpp

main_run: *.o
	$(CC) *.o -o main_run

comGroundManager: ARINC_Com.o statusManager.o comGroundManager.o
	$(CC) ARINC_Com.o statusManager.o comGroundManager.o -o main_Com_ST

Ground: ARINC_Com.o Ground.o
	$(CC) ARINC_Com.o Ground.o -o main_Ground

Control_run : ARINC_Com.o attitudeController.o cameraController.o control.o
	$(CC) ARINC_Com.o attitudeController.o cameraController.o control.o -o main_Control

kernel: kernel_arinc.cpp
	gcc kernel_arinc.cpp

clean:
	rm *.o *.gch

success:
	echo "\n[ Make successful! ]\n"

