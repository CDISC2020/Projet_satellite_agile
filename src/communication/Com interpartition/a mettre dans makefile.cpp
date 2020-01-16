WS_FDIR = src/FDIR/
WS_PF1 = src/PF1/
WS_PF2 = src/PF2/

$(MAKE) main_FDIR
$(MAKE) main_PF1
$(MAKE) main_PF2

main_FDIR: $(WS_FDIR)main_FDIR.cpp 
	$(CC) $(WS_FDIR)main_FDIR.cpp $(ARINC)ARINC_Com.cpp -o main_FDIR

main_PF1: $(WS_PF1)main_PF1.cpp 
	$(CC) $(WS_PF1)main_PF1.cpp $(ARINC)ARINC_Com.cpp -o main_PF1

main_PF2: $(WS_PF2)main_PF2.cpp 
	$(CC) $(WS_PF2)main_PF2.cpp $(ARINC)ARINC_Com.cpp -o main_PF2
