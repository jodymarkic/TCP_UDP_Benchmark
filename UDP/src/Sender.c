/*
*	FILENAME		: Sender.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds all methods used by UDP Sender, for both Windows
*					  and Linux enviroments
*
*/

#include "../inc/Proto.h"

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

// FUNCTIONS	: number_of_packets(void)
// DESCRIPTION	: This method retrieves user input for
//				  how many blocks they would like to send
// PARAMETERS	:  void
// RETURNS		: int : atoi(buffer)
int number_of_packets(void)
{
	char buffer[10] = "";
	printf("How many blocks would you like to send?\n");
	fgets(buffer, 10, stdin);
	trim(buffer);
	return atoi(buffer);
}

// FUNCTIONS	:  menu_handler()
// DESCRIPTION	: this method reads user input for the blocksize
//				  they chose in the block size menu
// PARAMETERS	: void
// RETURNS		: int : blockSize
int menu_handler()
{
	int blockSize = 0;
	char userInput[21]="";
	//display the block size menu
	sender_menu();
	fgets(userInput, 21, stdin);
	//trim carriage returns off of user input
    trim(userInput);
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
	return blockSize;
}

// FUNCTIONS	: indicating_block()
// DESCRIPTION	: this method concatinates the blockSize and the
//				  numOfBlocks in preparation for sending to the Reciever
// PARAMETERS	: int   : blockSize
//				  int   : numOfBlocks
// RETURNS		: char* : message
char* indicating_block(int blockSize, int numOfBlocks)
{
	char* message = NULL;
	message = (char*)malloc(sizeof(blockSize)+sizeof(numOfBlocks) + 1);
	sprintf(message, "%d|%d", blockSize, numOfBlocks);
    message[blockSize] = '\0'; //took the '\0' out of sprintf()
	return message;
}

// FUNCTIONS	: send_blocks()
// DESCRIPTION	: This method holds the main loops for sending all messages
//				  and blocks to the reciever
// PARAMETERS	: int				 : message_socket
//				  struct sockaddr_in : recieverInfo
//				  int 				 : numOfBlocks
//				  int 			     : blockSize
// RETURNS		:  void
void send_blocks(int message_socket, struct sockaddr_in recieverInfo, int numOfBlocks, int blockSize)
{
	//local variables
	char curTime[MESSAGESIZE];
#ifdef _WIN32
	SYSTEMTIME time;
#else
    struct timespec startTime;
#endif
	int ret = 0;
	char* pointersToBlocks[MESSAGESIZE];

	//allocate memory for all blocks going to be sent
	for (int i = 0; i < numOfBlocks; i++)
	{
		pointersToBlocks[i] = (char *)malloc(blockSize+1);
		//seed allocated memory with data to be sent
		generate_block(pointersToBlocks[i], blockSize, i);
	}
#ifdef _WIN32
	//get current time that measures milliseconds
	//acts as a start time
	GetLocalTime(&time);
#else
	//get current ime that will act as a start time
    clock_gettime(CLOCK_REALTIME, &startTime);
#endif
	//send all blocks
	for (int j = 0; j < numOfBlocks; j++)
	{
		//do
		//{
			ret = (int)sendto(message_socket, pointersToBlocks[j], (size_t)blockSize, 0, (struct sockaddr *)&recieverInfo, sizeof(recieverInfo));
		//} while (ret < blockSize);
	}
#ifdef _WIN32
	Sleep(1000);
	//calculate start time into milliseconds
	getTime(time, curTime);
#else
    sleep(1);
	//concatinate starttime's seconds and nanoseconds together
    sprintf(curTime, "%ld|%ld", startTime.tv_sec, startTime.tv_nsec);
#endif
	//send the start time to the reciever
    do
    {
        ret =  (int)sendto(message_socket, curTime, strlen(curTime), 0, (struct sockaddr *)&recieverInfo, sizeof(recieverInfo));
    } while (ret < 0);
	//sendto(message_socket, curTime, strlen(curTime), 0, (struct sockaddr *)&recieverInfo, sizeof(recieverInfo));
	printf("Jobs done!\n");
	//free all memory thats been allocated
	for (int i = 0; i < numOfBlocks; i++)
	{
		free(pointersToBlocks[i]);

	}
}

// FUNCTIONS	: generate_block()
// DESCRIPTION	: This method seeds block worth of data,
//				  it stamps the block with and count, and then
//                the remainding bytes
// PARAMETERS	: char* : block
//				  int   : block_size
//				  int   : num
// RETURNS		: char* : block
char* generate_block(char* block, int block_size, int num)
{
	int count = 48;

	//stamp block with a count
	sprintf(block, "%d|", num + 1);
	//generate remainding butes
	for (size_t i = strlen(block); i < block_size; i++)
	{
		if (count >= 58)
		{
			count = 48;
		}
		block[i] = count;
		count++;
	}
	//return the block
	return block;
}

// FUNCTIONS	: start_sending()
// DESCRIPTION	: This metod sends the initial message to the reciever
//				  once sent, it jumps into send_blocks() which sends the blocks
// PARAMETERS	: int 				 : mySocket
//				  int 				 : blockSize
//				  struct sockaddr_in : recieverInfo
// RETURNS		: void
void start_sending(int mySocket, int blockSize, struct sockaddr_in recieverInfo)
{
	char *indicatingBlk = NULL;
	int numOfBlocks = 0;
	int bytesSent = 0;
	//get the numberofBlocks
	if ((numOfBlocks = number_of_packets()) != 0)
	{
		//create the first message
		indicatingBlk = indicating_block(blockSize, numOfBlocks);
		do
		{
			//send the first message
			bytesSent = (int)sendto(mySocket, indicatingBlk, (int)strlen(indicatingBlk), 0, (struct sockaddr*)&recieverInfo, sizeof(recieverInfo));
		} while (bytesSent < 0);
		//send the blocks
		send_blocks(mySocket, recieverInfo, numOfBlocks, blockSize);
	}
}

// FUNCTIONS	: start_sender()
// DESCRIPTION	: this function starts the sender, it initializes
//				  a socket and recieves user input for the recievers
//				  ip address to be later used in sendto() function calls
// PARAMETERS	: int : port
// RETURNS		: int : retCode
int start_sender(int port)
{
	int mySocket;
	int blockSize = 0;
	int retCode = 2;
	char ip[128];

	struct sockaddr_in recieverInfo;

	//get receivers ip address
	printf("Enter the Server's ip address: ");
	fgets(ip, 20, stdin);
	trim(ip);
#ifdef _WIN32
	//store receivers ip address
	recieverInfo.sin_addr.s_addr = inet_addr(ip);
#else
	//get host name with ip
    struct hostent* host;
    host = gethostbyname(ip);
    memset(&recieverInfo, 0, sizeof(recieverInfo));
	//use host name to fill sin_addr used in sendto() calls
    memcpy(&recieverInfo.sin_addr, host->h_addr, host->h_length);
#endif

    recieverInfo.sin_family = AF_INET;
    recieverInfo.sin_port = htons((u_short)port);
	//create a socket
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // UDP socket
	if (mySocket >0)
	{
		retCode = 0;
		//get block size
        if ((blockSize = menu_handler()) >0)
        {
			//start_sending messages and blocks
            start_sending(mySocket, blockSize, recieverInfo);
        }
    }	/* endif */
#ifdef __linux__
	close(mySocket);
#endif
		return retCode;
}