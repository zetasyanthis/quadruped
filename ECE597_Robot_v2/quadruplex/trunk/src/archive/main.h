/*****************************************************************************************************
  *  Filename:			main.h
  *  Description:			Contains the function headers for main.c and the global variables and flags
  *					used by many functions.  
  *  Author:				Adam Hoffman, Mark Jacobson
  *  InitDate: 			May. 5, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#define STATIC_COMMAND_LENGTH 4
#define MOVE_MOTOR 0

void initialize(void);

extern unsigned char* path;
extern unsigned char* currentCommand;
extern unsigned char returnPacket[9];
extern unsigned char error;
extern unsigned char commandBuffer[9];
extern unsigned char pathLength;										// Contains number of complex commands
extern unsigned char complexLength;									// Contains number of bursts
extern unsigned char burstLengths[100];									// Contains number of commands per each burst
extern unsigned char duration[100];										// Contains the durations of each burst
extern unsigned char commandLength, returnLength;					// Contains the length of the current command and return packet
extern unsigned char complexNum, burstNum, commandNum, byteNum;		// Contains the currently referenced $var number
extern unsigned char timerCycles;
extern unsigned char newPath;
extern unsigned long time;
extern unsigned char blargTest;
extern unsigned char pathLength;
