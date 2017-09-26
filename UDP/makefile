# 
# Filename : makefile
# Project  : IADA01
# Date     : January 19,2017
# Author   : Nathan Bray, Jody Markic
# Description: 
#

# 
# =======================================================
#                  Linux Makefile
# =======================================================
CC = cc
LD =cc
CFLAGS = -c
LDFLAGS = -o
SRC=./src/
OBJ_PATH=./obj/
OBJ = ./obj/UDP.o ./obj/UserInterface.o ./obj/Receiver.o ./obj/Sender.o
STD_HEADERS = ./inc/Proto.h
EXE = ./bin/UDP_NBJM.exe

.PHONY: clean program
# FINAL BINARY Target

$(EXE): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(EXE)	
#
# =======================================================
#                  Dependencies
# =======================================================
$(OBJ_PATH)UDP.o: $(SRC)UDP.c $(SRC)UserInterface.c $(SRC)Receiver.c $(SRC)Sender.c $(STD_HEADERS)
	$(CC) $(CFLAGS) $(SRC)UDP.c           $(LDFLAGS) $(OBJ_PATH)UDP.o
	$(CC) $(CFLAGS) $(SRC)UserInterface.c $(LDFLAGS) $(OBJ_PATH)UserInterface.o
	$(CC) $(CFLAGS) $(SRC)Receiver.c	  $(LDFLAGS) $(OBJ_PATH)Receiver.o
	$(CC) $(CFLAGS) $(SRC)Sender.c	      $(LDFLAGS) $(OBJ_PATH)Sender.o

#
# =======================================================
# 					Other targets
# =======================================================  
# prepare for a complete rebuild						
clean:
	rm -f ./bin/*
	rm -f ./inc/*.h~
	rm -f $(OBJ_PATH)*.o
	rm -f $(OBJ_PATH)*.obj
	rm -f $(SRC)*.c~
	
# run the programs sender
send: $(EXE)
	chmod +x $(EXE)
	clear
	$(EXE) -s 3000

# run the programs reciever
recv: $(EXE)
	chmod +x $(EXE)
	clear
	$(EXE) -r 3000