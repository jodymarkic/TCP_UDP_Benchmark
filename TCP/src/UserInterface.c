/*
*	FILENAME		: UserInterface.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds  the functions that prompt for user input,
*					  menu displayas and timer and string manipulation functions used by
*					  the TCP project, for both Windows and Linux enviroments
*
*/

#include "../inc/Proto.h"


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

//
//	METHOD		: promptIP()
//	DESCRIPTION	: prompt user for the Servers IP address
//  PARAMETERS	: char* : ip
//	RETURNS		: char* : ip
//
char* promptIP(char * ip)
{
	/*
	* prompt for information
	*/
	do
	{
		printf("Enter the Server's ip address: \n");
		fgets(ip, 128, stdin);
		trim(ip);
		printf("\n");
	} while (ip == NULL || strlen(ip) == 0);
	return ip;
}

// FUNCTIONS	:  displayMenu()
// DESCRIPTION	: this method reads user input for the blocksize
//				  they chose in the block size menu, then creates
//				  a initial message to be sent to the server.
//				  it also calls send_blocks()
// PARAMETERS	: int : mySocket
// RETURNS		: int : blockSize
int displayMenu(int mySocket)
{
	int blockSize = 0;
	int numOfBlocks = 0;
	char userInput[21];
	//display menu
	sender_menu();
	fgets(userInput, 21, stdin);
	trim(userInput);
	//determine block size
	switch (atoi(userInput))
	{
	case 1:
		blockSize = 1000;
		break;
	case 2:
		blockSize = 2000;
		break;
	case 3:
		blockSize = 5000;
		break;
	case 4:
		blockSize = 10000;
		break;
	default:
		blockSize = -1;
		break;
	}

	char *indicatingBlk = NULL;
	if (blockSize != -1)
	{
		//get number of blocks
		if ((numOfBlocks = number_of_packets()) != 0)
		{
			//create initial message
			indicatingBlk = startConversation(blockSize, numOfBlocks);
			//send initial message
			send(mySocket, indicatingBlk, (int)strlen(indicatingBlk), 0);
			//send blocks
			send_blocks(mySocket, numOfBlocks, blockSize);
		}
	}
	return blockSize;
}

// FUNCTIONS	: sender_menu()
// DESCRIPTION	: This function just displays a block size menu
// PARAMETERS	: void
// RETURNS		: void
void sender_menu(void)
{
	printf("Please Select the Size of Block you would like to send.\n");
	printf("1. 1000 byte block\n");
	printf("2. 2000 byte block\n");
	printf("3. 5000 byte block\n");
	printf("4. 10000 byte block\n");
	printf("5. Shutdown Receiver\n");
}

// FUNCTIONS	: number_of_packets()
// DESCRIPTION	: This function prompts for the amount of blocks they'd
//				  like to send
// PARAMETERS	: void
// RETURNS		: void
int number_of_packets(void)
{
	char buffer[10] = "";
	printf("\nHow many packets would you like to send?\n");
	fgets(buffer, 10, stdin);
	trim(buffer);
	return  atoi(buffer);
}

#ifdef _WIN32
// FUNCTIONS	:  getTime()
// DESCRIPTION	: this method gets currenttime and converts it into milliseconds
//				  for the windows enviroment
// PARAMETERS	: SYSTEMTIME : time
//				  char*      : curTime
// RETURNS		: void
void getTime(SYSTEMTIME time, char* curTime)
{
	//curTime[0] = '\0';
	long time_ms = (time.wHour * 3600000) + (time.wMinute * 60000) + (time.wSecond * 1000) + time.wMilliseconds;
	sprintf(curTime, "%ld\0", time_ms);
}

// FUNCTIONS	: makeStartTime()
// DESCRIPTION	: The method takes a total time in milliseconds and converts
//				  it back into its hours, minutes, and seconds to store it into
//				  a SYSTEMTIME struct
// PARAMETERS	: int timeInMS
// RETURNS		: SYSTEMTIME time
SYSTEMTIME makeStartTime(int timeInMS)
{
	SYSTEMTIME time;
	int hour = timeInMS / 3600000;
	timeInMS = timeInMS % 3600000;
	int minute = timeInMS / 60000;
	timeInMS = timeInMS % 60000;
	int second = timeInMS / 1000;
	timeInMS = timeInMS % 1000;
	time.wHour = hour;
	time.wMinute = minute;
	time.wSecond = second;
	time.wMilliseconds = timeInMS;
	return time;
}
#else


#endif
// FUNCTIONS	: parseSMessage()
// DESCRIPTION	: this method takes blcoks recieved from the sender
//				  that has important data delimited with "|"
//				  it separates the data and returns the values via
//				  pointers
// PARAMETERS	: double* : one
//				  double* : two
//				  char*   : message
// RETURNS		: void
void parseSMessage(double* one, double*two, char* message)
{
char* token = NULL;
token = strtok(message, "|");
*one = atof(token);
while (token != NULL)
{
*two = atof(token);
token = strtok(NULL, "|");
}
}