/*
*	FILENAME		: UserInterface.c
*	PROJECT			: UDP_NBJM
*	PROGRAMMERS		: Nathan Bray & Jody Markic
*	FIRST VERSION	: 1/23/2017
*	DESCRIPTION		: This file holds timer and string manipulation functions used by the UDP project, for both Windows
*					  and Linux enviroments
*
*/
#include "../inc/Proto.h"

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

#endif

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
char* trim(char* string)
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

	return string;
}