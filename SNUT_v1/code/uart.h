/*****************************************************************************************************
  *  Filename:			uart.h
  *  Description:			Contains the function headers and global variables for uart.c.
  *  Author:				Adam Hoffman, Mark Jacobson
  *  InitDate: 			May. 5, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#define sendstring(x)		sendString(x,sizeof(x))
#define SET_BAUD_RATE1(x)	U1BRG = (2500000/x)-1
#define SET_BAUD_RATE2(x)	U2BRG = (10000000/x)-1
#define PACKET_NO_PARAMS 6
#define RS_485_DELAY 0.1			// Milliseconds of delay after setting the transmit state on RS-485 bus

void initUART(void);
void sendByte(char,char);
char getByte(char);
void sendHexAsASCII(char);
void sendString(char*, char);
void _ISR _U1TXInterrupt(void);
void _ISR _U1RXInterrupt(void);

extern unsigned char* nextCommand;
extern int motorPosition[12][2];
