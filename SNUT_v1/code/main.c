/*****************************************************************************************************
  *  Filename:			main.c
  *  Description:			Contains the main method and provides for the initialization of the robot and
  *					associated peripherals.
  *  Functions Contained:		main, initialize
  *  Author:				Adam Hoffman, Mark Jacobson
  *  InitDate: 			May. 5, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#define dsPIC33FJ256GP710
#include "includes.h"

unsigned char testString[] = {"\n\n\rSerial Port Initialized\n\r"};

unsigned char* path = NULL;
unsigned char* currentCommand = NULL;
unsigned char returnPacket[9];
unsigned char error;
unsigned char blargTest = 0;

unsigned char pathLength;
unsigned char commandBuffer[9];
unsigned char pathLength;								// Contains number of complex commands
unsigned char complexLength;								// Contains number of bursts
unsigned char burstLengths[100];						// Contains number of commands per each burst
unsigned char duration[100];
unsigned char commandLength, returnLength;				// Contains the length of the current command and return packet
unsigned char complexNum, burstNum, commandNum, byteNum;
unsigned char timerCycles;
unsigned char newPath;
unsigned long time;

_FOSCSEL(FNOSC_PRIPLL & IESO_OFF);				//  Selects primary oscilator with PLL, no two speed startup
_FOSC(FCKSM_CSDCMD & OSCIOFNC_ON & POSCMD_XT);	//  Selects no clock switching/monitoring, clock output pin enable, external clock
_FWDT(FWDTEN_OFF & WINDIS_OFF);					//  Disable watchdog timer and windowed watchdog timer.
_FPOR(FPWRT_PWR128);							//  Set power up reset for 128ms
_FICD(BKBUG_ON & JTAGEN_OFF);					//  Device resets in user mode rather than debug mode, JTAG off.

int main(void)
{
	// Set clock speed of processor
	PLLFBD=38; 						// M=40
	CLKDIVbits.PLLPOST=0; 			// N1=2
	CLKDIVbits.PLLPRE=0; 			// N2=2
	while(OSCCONbits.LOCK!=1);	 	// Wait for PLL to lock

	unsigned char i;

	initialize();					// initialize ports and settings

	commandBuffer[0] = 0xFF;
	commandBuffer[1] = 0xFF;
	
	for(i=0;i<12;i++)
		motorPosition[i][1] = (unsigned int)stand[i][2] + ((unsigned int)stand[i][3] << 8);

	pathLength = 1;
	pathSwitch(0);
	currentCommand = nextCommand;
	
	commandBuffer[2] = *(currentCommand+1);					// ID
	commandBuffer[3] = 5;									// Length = instruction + address + 2 byte angle + checksum = 5
	commandBuffer[4] = WRITE_DATA;							// Instruction
	commandBuffer[5] = 0x1E;								// Motor position address
	commandBuffer[6] = *(currentCommand+2);					// Low byte of angle
	commandBuffer[7] = *(currentCommand+3);					// High byte of angle
	commandBuffer[8] = addCheckSum(commandBuffer);			// Checksum
	commandLength = 9;
	returnLength = PACKET_NO_PARAMS;						// 6
	_RE0 = 1;												// Set transmit mode.
	wait_ms(RS_485_DELAY);
	
	U1TXREG = commandBuffer[byteNum];						// Begin command transmission 
	
	while(newPath == 0);
	
	
	IEC0bits.U1TXIE = 0;		// Disable UART1 Tx interrupt
	IEC0bits.U1RXIE = 0;		// Disable UART1 Rx interrupt
	_RE0 = 1;	
	wait_ms(1000);
	moveMotor(12,512);
	wait_ms(50);
	moveMotor(13,732);
	yPosition = 732;
	wait_ms(50);
	
	toggle = 0;
	pathLength = 8;
	path = realloc(path,pathLength*sizeof(unsigned char));
	path[0] = 0;
	path[1] = 1;
	path[2] = 2;
	path[3] = 2;
	path[4] = 2;
	path[5] = 2;
	path[6] = 2;
	path[7] = 2;
	while(1)
		spiralSearch();
	
	
	/*
	//TESTING CODE
	
	pathLength = 3;
	path = realloc(path,pathLength*sizeof(unsigned char));
	path[0] = 0;
	for(i=1;i<pathLength-1;i++) {
		path[i] = 1;
	}
	path[pathLength-1] = 2;
	pathSwitch(*path);
	currentCommand = nextCommand;
	
	// Buffer path to be sent... this may end up in a timer interrupt
	complexNum = 0;
	burstNum = 0;
	commandNum = 0;
	byteNum = 0;
	
	commandBuffer[2] = *(currentCommand+1);					// ID
	commandBuffer[3] = 5;									// Length = instruction + address + 2 byte angle + checksum = 5
	commandBuffer[4] = WRITE_DATA;							// Instruction
	commandBuffer[5] = 0x1E;								// Motor position address
	commandBuffer[6] = *(currentCommand+2);					// Low byte of angle
	commandBuffer[7] = *(currentCommand+3);					// High byte of angle
	commandBuffer[8] = addCheckSum(commandBuffer);			// Checksum

	commandLength = 9;
	returnLength = PACKET_NO_PARAMS;						// 6
	
	_RE0 = 1;												// Set transmit mode.
	wait_ms(RS_485_DELAY);
	
	U1TXREG = commandBuffer[byteNum];						// Begin command transmission
	*/
	
	while(1) {
		//PORTA = 0x01;
		wait_ms(1000);
		//PORTA = 0x80;
		wait_ms(1000);
	}
	
	// Exit Condition
	while(1);
}

//=================================================================================
// initialize
//=================================================================================
void initialize(void)
{
	TRISA = 0;				// Set port a as outputs.  
	ODCA = 0;
	TRISE = 0;				// Set port e as outputs.  
	
	initUART();				// Set up UARTs.
	initTimers();			// Set up timers.
	ecan1Init();			// ECAN1 Initialisation
	dma0init();				// Configure DMA Channel 0 for ECAN1 Transmit
	dma2init();				// Configure DMA Channel 2 for ECAN1 Receive
	//initAD();					// Set up A/D
	//initI2C();					// Set up I2C

	//Set up interrupts
	SRbits.IPL=0;				// CPU interrupt priority level is 0.
	INTCON1bits.NSTDIS = 1;		// Disable nested interrupts
	
	// Turn on indicator LEDs
	PORTA = 0xFF;
	
	_RA5 = 1;					// Turn off high-speed mode.

	wait_ms(1000);
	sendstring(testString);
	return;
}
