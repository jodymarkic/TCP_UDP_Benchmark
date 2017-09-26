/*
*	FILENAME		: Sender.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds all methods used by TCP Sender, for both Windows
*					  and Linux enviroments
*
*/

#include "../inc/Proto.h"


// FUNCTIONS	: startConversation
// DESCRIPTION	: This function create the first message to be sent to the
//				  reciever, it will hold blockSize, and number of blocks to send
// PARAMETERS	: int blockSize
//				  int numOfBlocks
// RETURNS		: char* : message
char* startConversation(int blockSize, int numOfBlocks)
{
	char* message = NULL;
	message = (char*)malloc(sizeof(blockSize)+sizeof(numOfBlocks) + 1);
	sprintf(message, "%d|%d", blockSize, numOfBlocks);
	return message;
}

// FUNCTIONS	: send_blocks()
// DESCRIPTION	: This method sends all blocks and the final message
//				  to the reciever
// PARAMETERS	: int message_socket
//				  int numOfBlocks
//				  int blockSize
// RETURNS		: void
void send_blocks(int message_socket, int numOfBlocks, int blockSize)
{
	int blocksSent = 0;
	int num = 0;
	char* pointersToBlocks[MESSAGESIZE];
	char curTime[MESSAGESIZE];
#ifdef _WIN32
	SYSTEMTIME time;
#else
    struct timespec startTime;
#endif
		
	//allocate and seed blocks to be sent
	for (int i = 0; i < numOfBlocks; i++)
	{
		pointersToBlocks[i] = (char *)malloc(blockSize+1);
		generate_block(pointersToBlocks[i], blockSize, i);
	}

	//get current time that measures milliseconds
#ifdef _WIN32
	//get starting time
	GetLocalTime(&time);
#else
    clock_gettime(CLOCK_REALTIME, &startTime);
#endif
	//send all  blocks to reciever
	for (int j = 0; j < numOfBlocks; j++)
	{
		do
		{
			num = send(message_socket, pointersToBlocks[j], blockSize - blocksSent, 0);
			if (num != SOCK_ERROR)
			{
				blocksSent += num;
			}
		} while (blocksSent < blockSize);
		blocksSent = 0;
	}

#ifdef _WIN32
	Sleep(1000);
	//
	getTime(time, curTime);
#else
    sleep(1);
    sprintf(curTime, "%ld|%ld", startTime.tv_sec, startTime.tv_nsec);
#endif
	//send final message which should hold start time
	send(message_socket, curTime, (int)strlen(curTime), 0);
	printf("Jobs done!\n");
	//free all allocated memory
	for (int i = 0; i < numOfBlocks; i++)
	{
		free(pointersToBlocks[i]);
	}
}


// FUNCTIONS	: generate_block()
// DESCRIPTION	: This function stamps a block with a count,
//				  and seeds the remainding bytes
// PARAMETERS	: char* block
//				  int block_size
//				  int num
// RETURNS		: char* block
char* generate_block(char* block, int block_size,int num)
{
	int count = 48;	

	sprintf(block, "%d|", num + 1);
	for (size_t i = strlen(block); i < block_size; i++)
	{
		if (count >= 58)
		{
			count = 48;
		}
		block[i] = count;
		count++;
	}
	block[block_size] = '\0';
	return block;
}