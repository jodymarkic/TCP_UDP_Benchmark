/*
*	FILENAME		: Proto.h
*	PROJECT			: TCP_NBJM
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file acts as a header file for the TCP Sender and Reciever.
*					  It holds all c libraries, constants and function prototypes to run
*					  in both Windows and Linux enviroments
*
*/

//libraries
#pragma once
#pragma warning(disable:4996)
#ifdef _WIN32
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"ws2_32")
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//constants
#define SERVER 1
#define CLIENT 2
#define MESSAGESIZE 100000
#define SCANSETMAXSIZE 121
#define TRIMLIST "\r\n\t"
#define SOCK_ERROR -1

//enum
typedef enum socketError
{
    GOOD,
    NOHOST,
    BINDERROR,
    CONNECTERROR
}socketError;


char* handleMessage(int message_socket, char* recievd, int blockSize);
int startSocketConnection(int appType, int port, enum socketError * retCode);
void parseSMessage(double* one, double*two, char* message);

// UI
void trim(char* string);
char* promptIP(char* ip);
void sender_menu(void);
int displayMenu(int mySocket);
int number_of_packets(void);
void printUsage();

//Sender
char* generate_block(char* block, int block_size, int num);
char* startConversation(int blocksize, int numOfBlocks);
void send_blocks(int message_socket, int numOfBlocks, int blockSize);

//Reciever
void listenForRequests(int mySocket);

//time functions
#ifdef _WIN32
SYSTEMTIME makeStartTime(int timeInMS);
void getTime(SYSTEMTIME realTime, char* curTime);
#endif
