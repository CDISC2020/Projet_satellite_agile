#!/bin/sh
.SUFFIXES : .c .o

# CONSTANTES
CC = g++ -Wall -std=c++11 -lpthread -pthread
MAKE = make

# ARINC directory
ARINC = src/ARINC/

# GPIO directory
GPIO = src/

# Partition directories
WP_PLAN = src/planManager/
WP_COM = src/communication/
WP_FDIR = src/FDIR/

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

genericInstruction: $(WP_PLAN)genericInstruction.cpp $(WP_PLAN)genericInstruction.h
	$(CC) -c $(WP_PLAN)genericInstruction.cpp $(WP_PLAN)genericInstruction.h

plan: $(WP_PLAN)plan.cpp $(WP_PLAN)plan.h
	$(CC) -c $(WP_PLAN)plan.cpp $(WP_PLAN)plan.h

planManager: $(WP_PLAN)planManager.cpp $(WP_PLAN)planManager.h $(WP_PLAN)main_PM.cpp $(WP_FDIR)FDIR.h
	$(CC) -c $(WP_PLAN)planManager.cpp $(WP_PLAN)planManager.h $(WP_PLAN)main_PM.cpp $(WP_FDIR)FDIR.h

comGroundManager: $(WP_COM)comGroundManager.cpp $(WP_FDIR)FDIR.h
	$(CC) -c $(WP_COM)comGroundManager.cpp $(WP_FDIR)FDIR.h

statusManager:  $(WP_COM)statusManager.cpp $(WP_COM)statusManager.h
	$(CC) -c $(WP_COM)statusManager.cpp $(WP_COM)statusManager.h

ground: $(WP_COM)Ground.cpp
	$(CC) -c $(WP_COM)Ground.cpp

attitudeController:  $(WP_PLAN)attitudeController.cpp $(WP_PLAN)attitudeController.h
	$(CC) -c $(WP_PLAN)attitudeController.cpp $(WP_PLAN)attitudeController.h

cameraController:  $(WP_PLAN)cameraController.cpp $(WP_PLAN)cameraController.h
	$(CC) -c $(WP_PLAN)cameraController.cpp $(WP_PLAN)cameraController.h

controller: $(WP_PLAN)Controller.cpp $(WP_PLAN)Controller.h
	$(CC) -c $(WP_PLAN)Controller.cpp $(WP_PLAN)Controller.h

gpio : $(GPIO)GPIO.cpp $(GPIO)GPIO.h
	$(CC) -c $(GPIO)GPIO.cpp $(GPIO)GPIO.h

fdir: $(WP_FDIR)FDIR.cpp $(WP_FDIR)FDIR.h $(WP_FDIR)watchdog_arduino.cpp $(WP_FDIR)watchdog_arduino.h $(WP_FDIR)WatchdogInterne.cpp $(WP_FDIR)WatchdogInterne.h $(WP_FDIR)main_FDIR.cpp
	$(CC) -c $(WP_FDIR)FDIR.cpp $(WP_FDIR)FDIR.h $(WP_FDIR)watchdog_arduino.cpp $(WP_FDIR)watchdog_arduino.h $(WP_FDIR)WatchdogInterne.cpp $(WP_FDIR)WatchdogInterne.h $(WP_FDIR)main_FDIR.cpp


############################### LINKAGE ####################################

##############
# Partitions #
##############
main_PM: ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o main_PM.o Controller.o 
	$(CC) ARINC_Com.o genericInstruction.o plan.o planManager.o statusManager.o attitudeController.o cameraController.o GPIO.o main_PM.o Controller.o -o main_PM

main_com: ARINC_Com.o statusManager.o comGroundManager.o
	$(CC) ARINC_Com.o statusManager.o comGroundManager.o -o main_Com_ST

main_FDIR: ARINC_Com.o main_FDIR.o FDIR.o WatchdogInterne.o watchdog_arduino.o GPIO.o
	$(CC) ARINC_Com.o main_FDIR.o FDIR.o WatchdogInterne.o watchdog_arduino.o GPIO.o -o main_FDIR

######################
# Station Sol Ground #
######################
main_Ground: ARINC_Com.o Ground.o
	$(CC) ARINC_Com.o Ground.o -o main_Ground

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
