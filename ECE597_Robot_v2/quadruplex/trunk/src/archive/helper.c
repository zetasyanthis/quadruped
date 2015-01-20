/*****************************************************************************************************
  *  Filename:			helper.c
  *  Description:			Contains misc. helper methods used in other areas of the code.  Some are not used
  *					except for rudamentary debugging.  
  *  Functions Contained:		wait_ms, criticalError, burstLengthSetup, addCheckSum, abs
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#include "includes.h"

//=================================================================================
// waits ~1 second
//=================================================================================
void wait_ms(float delay_ms)
{
	volatile float i,j,k, delay, blarg;
	delay = 1000/delay_ms;
	for(i = 0; i < 1000/delay; i++)
	{
		for(j = 0; j < 6; j++)
		{
			for(k = 0; k < 10; k++)
			{
				blarg = (i/j)/k;
				Nop(); 	// wait at least 9 usec (1/115200)
			}
		}
	}
}

//=================================================================================
// criticalError
//
//		Infinitely loops and blinks LEDs to show an error has occured.
//
//=================================================================================
void criticalError(void) {

	while(1) {
		wait_ms(1000);
		//PORTA = 0x0F;
		wait_ms(1000);
		//PORTA = 0xF0;
	}
}

//=================================================================================
// burstLengthSetup
//
//		Cycles through next complex command and determines burstlengths
//
//=================================================================================
void burstLengthSetup(int totalLength) {

	unsigned char i = 0;
	unsigned char iPrev = 0;
	unsigned char *temp = nextCommand;

	complexLength = 1;
	duration[0] = nextCommand[0];

	for(i=1;i < totalLength;i++) {
		nextCommand += STATIC_COMMAND_LENGTH;
		if((nextCommand[0] != 0) || (i == totalLength)) {
			burstLengths[complexLength-1] = i - iPrev;
			iPrev = i;
			complexLength++;
			duration[complexLength-1] = nextCommand[0];
		}
	}
	
	burstLengths[complexLength-1] = totalLength - iPrev;
	nextCommand = temp;

	return;
}

//=================================================================================
// addCheckSum
//
//		Adds the checksum of a packet by adding all elements of the packet except
//		the start flag, then negating.
//
//=================================================================================
char addCheckSum(unsigned char* byteArray)
{
	char i;
	int sum = 0;
	char lng;
	lng = byteArray[3] + 3;		//Get length from byte array, add 3 for offset

	for(i=2;i<lng;i++)
	{
		sum += byteArray[i];		//Add all bytes in packet except start flags
	}
	sum = (0xFF & sum); 			//Mask to get last byte only
	sum = (~sum);					//Negate by bitwise negating and adding 1
	return (char) (sum & 0xFF);
}

//=================================================================================
// abs
//
//		Takes the absolute value.
//
//=================================================================================

int abs(int number) {
	if(number < 0)
		number *= -1;
	return number;
}

//=================================================================================
// moveMotor
//
//		Moves a motor to a goal position
//
//=================================================================================
int moveMotor(unsigned char motorID,int position)
{
	int fail;
	
	char numParams = 3;
	char posH = (position & 0xFF00) >> 8;
	char posL = (position & 0x00FF);
	char params[3] = {0x1E,posL,posH};
	
	fail = sendPacket(motorID,WRITE_DATA,numParams,params);	//Make the command into a packet and send
	
	return fail;
}

//=================================================================================
// sendPacket
//
//		Create a packet from the given parameters and send it out on serial port
//
//=================================================================================
int sendPacket(unsigned char motorID, char instrID, char numParams, char* params)
{
	char* command;	
	int i;
	
	//Allocate appropriate amount of memory for command array and fill it
	command = (char*)malloc(PACKET_NO_PARAMS+numParams);
	command[0] = 0xFF;						//Start flag
	command[1] = 0xFF;						//Start flag
	command[2] = motorID;					//ID
	command[3] = 2+numParams;				//length
	command[4] = instrID;					//instruction
	for(i=0;i<numParams;i++)				//Parameters
	{
		command[5+i]=params[i];
	}
	command[5+numParams] = addCheckSum(command);//checksum
	
	//Set rs-485 shifter to send
	sendCommand(command);						//Send the command to the motor
	
	free(command);
	return 0;												//  Pass error to higher level function
}

//=================================================================================
// sendCommand
//
//		Sends a command via the serial port.  If in debug mode, this prints the
//		value of the data as text
//
//=================================================================================
void sendCommand(char* command)
{
	char commandLength;
	char i;
	
	commandLength = command[3]+4;		//length + 2 start flags + ID + checksum
	for(i=0;i<commandLength;i++)		//Send bytes one at a time
	{
		sendByte(command[i],1);
	}
	
	return;
}
