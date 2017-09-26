/*
*	FILENAME		: UDP.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds the main used by the UDP project, for both Windows
*					  and Linux enviroments
*
*/

#include "../inc/Proto.h"

int main(int argc, char *argv[])
{
	//local variables
    int retCode = 0;
	int port = 0;
#ifdef _WIN32
	WSADATA wsaData;
	int mySocket = 0;
#endif

	//check out command line arguements
	switch (argc)
	{
	case 3:
		//store argv2 into the port variable
		port = atoi(argv[2]);
		if (port <= 0)
		{
			printf("Invalid Port");
			printUsage();
			break;
		}
		// Intentional Fallthrough
	case 2:
		//if no port is given default is 13000
		if (argc == 2)
		{
			port = 13000;
		}
#ifdef _WIN32
		//setup windows environment to allow and use socket communications
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != SOCK_ERROR)
		{
#endif
			//check to see argv is -r or -s this determines if
			//the current executable will run as a sender or reciever
			if (strcmp(argv[1], "-r") == 0)
			{
				retCode = start_reciever(port);
			}
			else if (strcmp(argv[1], "-s") == 0)
			{
				retCode = start_sender(port);
			}
			else
			{
				printf("Incorrect switch provided!\n");
				printUsage();
			}
#ifdef _WIN32
			//close socket and WSACleanup
        closesocket((SOCKET)mySocket);
        WSACleanup();

		}

    else
		{
			printf("Socket could not be created.");
		}
#endif
		break;
	default:
		printUsage();
		break;
	}
	return retCode;

}


//
//	METHOD		: printUsage()
//	DESCRIPTION	: If the wrong amount of commandline args give
//				  give user input for the types of switches they
//				  can use for a proper setup
//  PARAMETERS	: void
//	RETURNS		: void
//
void printUsage()
{
	printf("Please enter either a \"-s\" for Server or a \"-c\" for Client\n");
	printf("Press any key to continue...");
	getchar();
}