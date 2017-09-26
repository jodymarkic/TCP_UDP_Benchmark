/*
*	FILENAME		: Receiver.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds all methods used by UDP Reciever, for both Windows
*					  and Linux enviroments
*
*/

#include "../inc/Proto.h"

// FUNCTIONS	: listenForRequests()
// DESCRIPTION	: This method takes a socket, and begin listening for requests from the sender
//			      it recieves an initial message that holds the values for block size to expect, along
//				  with the number of blocks being sent.
// PARAMETERS	: int : mySocket : the recievers socket
// RETURNS		: void
void listenForRequests(int mySocket)
{
	//local variables
	int timeout_flag = 0;
	int missingData = 0;
	double numberOfBlocks = -1;
	int actualRecievedBlocks = 0;
	double blockSize = 0;
	char message[SMALLMESSAGE] = "";
	char * recievedBlocks[MESSAGESIZE];
	char* token = NULL;
	int socketInfoLen;
	struct sockaddr_in senderInfo;


#ifdef _WIN32
	//windows variable
	SYSTEMTIME endTime, startTime;
#else
	// linux variables
    struct timespec endTime;
#endif
	char smallMessage[MESSAGESIZE];

	socketInfoLen = sizeof(senderInfo);
    int num = 0;
	// recieve out initial message that should hold the blocksize and amount of blocks that
	// are about to be sent
    do {
        num = (int) recvfrom(mySocket, message, SMALLMESSAGE, 0, (struct sockaddr *) &senderInfo, &socketInfoLen);
    }while (num < 0);

	//parse the initial message so we can store
	//and reuse the blocksize and amount of blocks data
    parseSMessage(&blockSize, &numberOfBlocks, message);

	//allocate memory based on the blocksize and amount of data
	//were about to recieve
	for (int i = 0; i < numberOfBlocks; i++)
	{
		recievedBlocks[i] = (char *)malloc(blockSize +1);
	}

	//start listening and recieving blcoks
	for (int i = 0; i < numberOfBlocks; i++)
	{
		//store the current recieved block into memory that we've allocated
        recievedBlocks[i] = handle_communications(mySocket, recievedBlocks[i], blockSize, &timeout_flag, i);
        actualRecievedBlocks = i + 1;
		// if we've recieved a timeout, break out of the recieve
		// loop
        if (timeout_flag == 1)
		{
			break;
		}
	}
	//stop-timer
#ifdef _WIN32
	// get the current time
	//that will act as our stop time
	GetLocalTime(&endTime);
#else
	//get the current time
	//that will act as our stop time
    clock_gettime(CLOCK_REALTIME, &endTime);
#endif
	//recieve our final message from the sender that should hold
	// the start time
    int numming = -1;
    do
    {
        numming = (int)recvfrom(mySocket, smallMessage, MESSAGESIZE, 0, (struct sockaddr *)&senderInfo, &socketInfoLen);
    }
    while(numming < 0);

	int blockStamp = 0;
	int outOfOrder = 0;
    double diff = 0;
	smallMessage[numming] = '\0';
	//trim our message of carriage returns
	trim(smallMessage);
    char time[MESSAGESIZE];
#ifdef _WIN32
	//start converting the starttime to a int
	//so we can do calculations on it
	int startInMS = atoi(smallMessage);
	startTime = makeStartTime(startInMS);
	//convert our end time into a number we can do
	//calculations on
	getTime(endTime, time);
	int tester = atoi(time);
	//calculate duration from our start and stop time
	diff = (tester - startInMS);
#else
    double secStart = 0;
    double nanoStart = 0;
	//trim out message of carriage returns
    parseSMessage(&secStart,&nanoStart, smallMessage);
	//calculate our durations from our start and stop time
    diff = ((endTime.tv_sec - secStart)*1000)+
            ((endTime.tv_nsec - nanoStart)/1000000);
#endif
	//check for out of order data
	for (int i = 1; i < actualRecievedBlocks-1; i++)
	{
		blockStamp = atoi(strtok(recievedBlocks[i - 1], "|"));
		//if the current block in question is greater than the next block in sequence
		//the data is out of order.
		if ((blockStamp != i) && (blockStamp > atoi(strtok(recievedBlocks[i], "|"))))
		{
			outOfOrder++;
		}
	}
	//calculate the total amount of missing blocks
	if (numberOfBlocks != actualRecievedBlocks)
	{
		missingData = (int)numberOfBlocks - actualRecievedBlocks;
	}
	//convert our duration from milliseconds to seconds
    diff /=1000;
	printf("Done.\n\n");
#if _WIN32
	//display our results
	printf("Time Started Sending: %02d:%02d:%02d:%03d\n", startTime.wHour, startTime.wMinute, startTime.wSecond, startTime.wMilliseconds);
	printf("Time Finished Recieving: %02d:%02d:%02d:%03d\n", endTime.wHour, endTime.wMinute, endTime.wSecond, endTime.wMilliseconds);
#endif
	//display our results
	printf("Time Elapsed (in seconds): %.03f\n\n", diff);
	printf("Size Of Blocks Sent: %d\n", (int)blockSize);
	printf("Blocks Sent: %d\n", (int)numberOfBlocks);
	printf("Blocks Recieved: %d\n", actualRecievedBlocks);
	printf("Missing Data: %d\n", missingData);
	printf("Out of Order Data: %d\n\n", outOfOrder);
	printf("Speed:%f bytes/s\n", (numberOfBlocks*blockSize) / (diff));

	int i = 0;
	printf("Done.\n\n");
	//free all allocated memory
	for (i = 0; i < numberOfBlocks; i++)
	{
		free(*(recievedBlocks + i));
	}
}


// FUNCTIONS	: handle_communications()
// DESCRIPTION	: This function recieves one block at a time and sets a timeout
//				  incase of getting stuck on the recvfrom(). It takes the recieved block
//				  and stores it into memory for later use. If the timeout is triggered
//				  it sets an int flag to stop further processing of blocks recieved.
// PARAMETERS	: int   : message_socket
//				  char* : recievd
//			      int 	: blockSize
//			      int*  : timeout_flag
//				  int   : count			
// RETURNS		: char* : recievd
char* handle_communications(int message_socket, char* recievd, int blockSize, int* timeout_flag, int count)
{
	//local variables
	struct sockaddr_in senderInfo;
	int socketInfoLen= sizeof(senderInfo);
	char* bytesRecieved = NULL;
    int retCode = 0;
#ifdef _WIN32
	// seed values used with the timeout
	int sock_timeout = 1000;
#else
	// seed values used with the timeout
	struct timeval sock_timeout;
	sock_timeout.tv_sec = 1;
	sock_timeout.tv_usec = 0;
#endif
	//allocate memory for recieving a block
	bytesRecieved = (char *)malloc(blockSize + 1);
	recievd[0] = '\0';
	//set a timeout for 1 second if no data is recieved
	setsockopt(message_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&sock_timeout,
		sizeof(sock_timeout));

	// null terminate our allocated memory
	bytesRecieved[0] = '\0';
	do
	{	//recieve a block from the sender
		if ((retCode = recvfrom(message_socket, bytesRecieved, blockSize, 0, (struct sockaddr *)&senderInfo, &socketInfoLen)) > 0)
		{
			//if we recieve a block store it into memory
			strcpy(recievd, bytesRecieved);
			break;
		}
		else
		{	//if were not on the first block recieved, and the timeout triggers
			//set a flag to stop all further recieving of blocks
			//(used outside this function)
			if (count > 0)
			{
				*timeout_flag = 1;
				break;
			}
		}
	} while (1);

	//free the memory acting as temporary storage for the 
	// block recieved
	free(bytesRecieved);

	//place a null terminator at the end of the block
	recievd[blockSize] = '\0';
	//return the block recieved
	return recievd;
}



// FUNCTIONS	: parseSMessage()
// DESCRIPTION	: this method takes blcoks recieved from the sender
//				  that has important data delimited with "|"
//				  it separates the data and returns the values via
//				  pointers
// PARAMETERS	: double* : one
//				  double* : two
//				  char*   : message
// RETURNS		: void
void parseSMessage(double* one, double* two, char* message)
{
    char* token = NULL;
    token = strtok(message, "|");
    *one = atof(token);
    while(token != NULL)
    {
        *two = atof(token);
        token = strtok(NULL, "|");
    }
}

// FUNCTIONS	: start_reciever()
// DESCRIPTION	: This method starts the Reciever, it
//				  creates a socket and then bind that socket to
//				  an endpoint. Once both are done it moves into
//			      listenForRequests()
// PARAMETERS	: int : port
// RETURNS		: int : retCode
int start_reciever(int port)
{
	int mySocket;
	int retCode = 0;
	struct sockaddr_in socketInfo;

	//timeout.tv_usec = 0;

	//seed socket information
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_port = htons(port);
	socketInfo.sin_addr.s_addr = htonl(INADDR_ANY);

	//create a socket
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // UDP socket
	if (mySocket == SOCK_ERROR)
	{
		//*retCode = 2;
	}	/* endif */

	//bind the socket to an endpoint
	int bind_value = bind(mySocket, (struct sockaddr*)&socketInfo, sizeof(socketInfo));
	if (bind_value == SOCK_ERROR) {
		//*retCode = 3;
	}	/* endif */

	//start listening for requests and blocks
	listenForRequests(mySocket);
#ifdef __linux__
	close(mySocket);
#endif
	//we'll use this to not execute recieves if anything is wrong with socket startup. 
	return retCode;
}