/*
*	FILENAME		:Proto.h
*	PROJECT			: UDP_NBJM
*	FIRST VERSION	:1/23/2017
*	DESCRIPTION		: This file acts as a header file for the UDP Sender and Reciever.
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
#include <netdb.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//constants
#define SOCK_ERROR -1
#define MESSAGESIZE 100000
#define SMALLMESSAGE 1001
#define SCANSETMAXSIZE 121
#define TRIMLIST "\r\n\t"


//shared methods
void printUsage();
//int startSocketConnection(int appType, int port, int* retCode);

//receiver methods
char* handle_communications(int message_socket, char* recievd, int blockSize, int* timeout_flag, int count);
void listenForRequests(int mySocket);
int start_reciever(int port);

//sender methods
void sender_menu(void);
int number_of_packets(void);
int menu_handler();
char* indicating_block(int blockSize, int numOfBlocks);
void send_blocks(int message_socket, struct sockaddr_in recieverInfo, int numOfBlocks, int blockSize);
char* generate_block(char* block, int block_size, int num);
void start_sending(int mySocket, int blockSize, struct sockaddr_in recieverInfo);
int start_sender(int port);

//timers
char* trim(char* string);

void parseSMessage(double* one, double* two, char* message);

#ifdef _WIN32
SYSTEMTIME makeStartTime(int timeInMS);
void getTime(SYSTEMTIME realTime, char* curTime);
#endif
