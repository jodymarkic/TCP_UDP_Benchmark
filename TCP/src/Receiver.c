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
// DESCRIPTION	: This method handles all incoming requests and blocks
//				  from the semder
// PARAMETERS	: int mySocket
// RETURNS		: void
void listenForRequests(int mySocket)
{
	//locals
	double numberOfBlocks = 0;
	double blockSize = 0;
	char* receivedBlocks[MESSAGESIZE];
	char smallMessage[MESSAGESIZE];
	double diff = 0;
	int fromLen;
	int message_socket;
	struct sockaddr_in from;
#ifdef _WIN32
    int startInMS=0;
	SYSTEMTIME endTime;
	SYSTEMTIME startTime;
#else
    struct timespec endTime;
#endif
	/*
	* accept() will block waiting for an incoming request.
	*/
	fromLen = sizeof(from);
	//accept incoming requests
	message_socket = (int)accept(mySocket,
		(struct sockaddr*)&from, &fromLen);

	if (message_socket >0)
	{
		//recieve the starting message
		int num =(int)recv(message_socket, smallMessage, MESSAGESIZE, 0);
		smallMessage[num] = '\0';
		//trim request
		trim(smallMessage);
		//store the block size and number of blocks
        parseSMessage(&blockSize, &numberOfBlocks, smallMessage);

		
		if (numberOfBlocks != 0 && blockSize != 0)
		{
			//allocate memory for all incoming blocks
			for (int i = 0; i < numberOfBlocks; i++)
			{
				receivedBlocks[i] = (char *)malloc((size_t)blockSize + 1);
			}
			//recieve all incoming blocks
			for (int i = 0; i < numberOfBlocks; i++)
			{
				receivedBlocks[i] = handleMessage(message_socket, receivedBlocks[i], (int)blockSize);
			}
#ifdef _WIN32
			//get our end time
			GetLocalTime(&endTime);
#else
            clock_gettime(CLOCK_REALTIME, &endTime);
#endif
			//recieve the senders start time
            num =(int)recv(message_socket, smallMessage, MESSAGESIZE, 0);
			smallMessage[num] = '\0';
			//trim the start time
			trim(smallMessage);
#ifdef _WIN32
			//convert the start time into a workable value
            startInMS = atoi(smallMessage);
			startTime = makeStartTime(startInMS);
			getTime(endTime, smallMessage);
			int tester = atoi(smallMessage);
			//get the duration with the endtime - the starttime
            diff = (tester - startInMS);
#else
            double secStart = 0;
            double nanoStart = 0;
            parseSMessage(&secStart, &nanoStart, smallMessage);
            diff = ((endTime.tv_sec - secStart)*1000)+
            ((endTime.tv_nsec - nanoStart)/1000000);

#endif
			//convert durationg into milliseconds
            diff /= 1000;
			//display results
			printf("Done.\n\n");
#ifdef _WIN32
			printf("Time Started Sending: %02d:%02d:%02d:%03d\n",startTime.wHour, startTime.wMinute, startTime.wSecond, startTime.wMilliseconds);
			printf("Time Finished Recieving: %02d:%02d:%02d:%03d\n", endTime.wHour, endTime.wMinute, endTime.wSecond, endTime.wMilliseconds);
#endif
			printf("Time Elapsed (in seconds): %.03f\n\n", diff);
			printf("Size of Block: %d\n",(int)blockSize);
			printf("Blocks Sent: %d\n",(int)numberOfBlocks);
			printf("Speed: %f bytes/s\n", (numberOfBlocks*blockSize) / diff);

			//free all allocated memory
			for (int i = 0; i < numberOfBlocks; i++)
			{
				free(receivedBlocks[i]);
			}
		}
	}
}


// FUNCTIONS	: handleMessage()
// DESCRIPTION	: this method recieves one block at a time and stores it into
//				  memory
// PARAMETERS	: int : message_socket
//				  char* : recievd
//				  int : blockSize
// RETURNS		: char* recievd
char* handleMessage(int message_socket, char* recievd, int blockSize)
{
	int currentBytes = 0;
	int retCode = 0;
	char* bytesRecieved = NULL;
	//allocate memory for temporary storage
	bytesRecieved = (char *)malloc((size_t)blockSize +1);
	recievd[0] = '\0';
	do {
		bytesRecieved[0] = '\0';
		//assign block to temporary storage
		retCode = (int)recv(message_socket, bytesRecieved, (size_t)(blockSize-currentBytes), 0);
		if (retCode != SOCK_ERROR)
		{
			if (retCode != blockSize)
			{
				bytesRecieved[retCode] = '\0';
			}

			currentBytes += retCode;
			//place block into permanent storage
			strcat(recievd, bytesRecieved);
		}
		else
		{
			printf("Got that SOCKET_ERROR blues.\n");
		}
	} while (currentBytes < blockSize);

	//free allocated memory to temporary storage
	free(bytesRecieved);

	return recievd;
}	/* end handle_communications */