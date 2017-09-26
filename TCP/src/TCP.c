/*
*	FILENAME		: TCP.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds the main used by the TCP project, for both Windows
*					  and Linux enviroments
*
*/
#include "../inc/Proto.h"

int main(int argc, char *argv[])
{
	//local variables
	int port = 0;
#ifdef _WIN32
	WSADATA wsaData;
#endif
	int mySocket;

	//check out command line arguements
	switch (argc)
	{
	case 3:
		//store argv2 into the port variable
		port = atoi(argv[2]);
		if (port <= 0)
		{
			printf("Invalid Port\n");
			printUsage();
			break;
		}

		// Intentional Fallthrough
	case 2:
		if (argc == 2)
		{
			//if no port is given default is 13000
			port = 13000;
		}
#ifdef _WIN32
		// Initiate the use of Winsock
		if (WSAStartup(0x202, &wsaData) != SOCKET_ERROR)
		{
#endif
			enum socketError  retCode = GOOD;
			//check to see argv is -r or -s this determines if
			//the current executable will run as a sender or reciever
			if (strcmp(argv[1], "-s") == 0)
			{
				mySocket = startSocketConnection(SERVER, port, &retCode);
				if (retCode == GOOD)
				{

					listenForRequests(mySocket);
				}
			}
			else if (strcmp(argv[1], "-c") == 0)
			{
				mySocket = startSocketConnection(CLIENT, port, &retCode);
				if (retCode == GOOD)
				{
					displayMenu(mySocket);
				}
			}
			else
			{
				printf("Incorrect switch provided!\n");
				printUsage();
			}
#ifdef _WIN32
			//close socket and WSACleanup
			closesocket(mySocket);
			WSACleanup();
		}
		else
		{
			printf("Socket could not be created.\n");
		}
#else
		close(mySocket);
#endif
		break;
	default:
		printUsage();
		break;
	}
	return 0;
}	/* end main */


//
//	METHOD		: startSocketConnection()
//	DESCRIPTION	: sets up the Socket appropriately depending on whether
//				  the executable is either the client of server.
//  PARAMETERS	: int : appType
//				  int : port
//				  enum socketError* : retCode
//	RETURNS		: int : mySocket
//
int startSocketConnection(int appType, int port, enum socketError * retCode)
{
	int mySocket = 0;

	char ip[128];
	unsigned int addr = 0;
	struct hostent *hp;
	struct sockaddr_in socketInfo;

	if (appType == SERVER)
	{
		// use INADDR_ANY for the socket address
		socketInfo.sin_addr.s_addr = INADDR_ANY;
	}
	else if (appType == CLIENT)
	{
		//prompt user for the server ip
		promptIP(ip);

		/*
		* resolve the ip address (in case it's a name rather than a number)
		*/
		addr = inet_addr(ip);
		if (addr == INADDR_NONE)
		{
			hp = gethostbyname(ip);
			if (hp == NULL) 
			{
				*retCode = NOHOST;
			}	/* endif */
			else
			{
				addr = *((unsigned int *)hp->h_addr_list[0]);
			}
		}	/* endif */


		memset(&socketInfo, 0, sizeof(socketInfo));
		socketInfo.sin_addr.s_addr = addr;
	}


	if (*retCode == GOOD)
	{
		socketInfo.sin_family = AF_INET;
		socketInfo.sin_port = htons(port);
		//create a socket
		mySocket = (int)socket(AF_INET, SOCK_STREAM, 0); // TCP socket

		if (mySocket > 0)
		{

			if (appType == SERVER)
			{
				/*
				* bind() associates a local address and port combination with the
				* listener socket just created.
				*/
				if (bind(mySocket, (struct sockaddr*)&socketInfo, sizeof(socketInfo)) == SOCK_ERROR)
				{
					*retCode = BINDERROR;
				}	/* endif */
				else
				{
					//start listening at the bound endpoint
					if (listen(mySocket, SOMAXCONN) == SOCK_ERROR)
					{
						*retCode = CONNECTERROR;
					}	/* endif */
				}
			}
			else if (appType == CLIENT)
			{
				//if a client connect to that endpoint
				if (connect(mySocket, (struct sockaddr *)&socketInfo, sizeof(socketInfo)) == SOCK_ERROR)
				{
					*retCode = CONNECTERROR;
				}
			}
		}
	}
	return mySocket;
}


/*
* Function    : char* trim
* Description : This function can remove the whitespace on either end of a string parameter.
*				It will also take a non-delimited list of other single characters you
*				can trim using the defined TRIMLIST string to for example, remove
*				the newline character at the end of a string.
*               This resource came from https://github.com/Jarlaxe227/C-Trim-Function/blob/master/trim.c
*
* Parameter   : char* string - This is the string to be chacked and possibly trimmed
* Returns     : char* string - This is the same string, after the trim function has completed
*/
void trim(char* string)
{
	if (string != NULL && strlen(string) > 0)
	{
		char scanset[SCANSETMAXSIZE] = "";

#ifdef TRIMLIST		
		sprintf(scanset, "%%[%s ]", TRIMLIST);
#endif // TRIMLIST

#ifndef TRIMLIST
		sprintf(scanset, "%%[ ]");
#endif // No TRIMLIST


		char tempString[SCANSETMAXSIZE] = "";
		char* frontPtr = string;
		char* backPtr = &string[strlen(string) - 1];

		// Move pointer forward until trim character is no longer found
		while ((sscanf(frontPtr, scanset, &tempString) == 1) && frontPtr < backPtr)
		{
			frontPtr++;
		}
		// Same idea, but at the back of the string
		while ((sscanf(backPtr, scanset, &tempString) == 1) && backPtr >= frontPtr)
		{
			backPtr--;
		}

		// Move the back pointer up one and put a null terminator there, cutting off the string
		backPtr++;
		*backPtr = '\0';

		// Then just copy the string from where the front pointer stopped as the final result
		strcpy(string, frontPtr);
	}

	//return string;
}

