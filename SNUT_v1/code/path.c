/*****************************************************************************************************
  *  Filename:			path.c
  *  Description:			Contains path generation functions and complex command arrays.  
  *  Functions Contained:		generatePath, sonarScan, pathSwitch
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#include "includes.h"

unsigned int toggle = 0;
unsigned int xPosition, yPosition;

const unsigned char stand[12][4]={
		{100,0,0x34,0x2},
		{0,1,0x81,0x1},
		{0,2,0xFD,0x1},
		{0,3,0xCC,0x1},
		{0,4,0x7F,0x2},
		{0,5,0x3,0x2},
		{0,6,0x34,0x2},
		{0,7,0x81,0x1},
		{0,8,0xFD,0x1},
		{0,9,0xCC,0x1},
		{0,10,0x7F,0x2},
		{0,11,0x3,0x2}};
		
const unsigned char walk[84][4]={ 
		{100,0,0x34,0x2}, 
		{0,1,0x2,0x2}, 
		{0,2,0xB1,0x1}, 
		{0,3,0xCC,0x1}, 
		{0,4,0x7F,0x2}, 
		{0,5,0x3,0x2}, 
		{0,6,0x34,0x2}, 
		{0,7,0x81,0x1}, 
		{0,8,0xFD,0x1}, 
		{0,9,0xCC,0x1}, 
		{0,10,0xA8,0x2}, 
		{0,11,0x4F,0x2}, 
		{100,0,0x85,0x2}, 
		{0,1,0xC8,0x1}, 
		{0,2,0x38,0x2}, 
		{100,1,0x37,0x1}, 
		{0,2,0x75,0x2}, 
		{100,1,0xF4,0x0}, 
		{0,2,0x38,0x2}, 
		{100,0,0x34,0x2}, 
		{0,1,0x58,0x1}, 
		{0,2,0xB1,0x1}, 
		{100,9,0x7B,0x1}, 
		{0,10,0xC,0x3}, 
		{0,11,0xC8,0x1}, 
		{100,10,0xC9,0x2}, 
		{0,11,0x8B,0x1}, 
		{100,10,0x38,0x2}, 
		{0,11,0xC8,0x1}, 
		{100,9,0xCC,0x1}, 
		{0,10,0xFE,0x1}, 
		{0,11,0x4F,0x2}, 
		{100,1,0x5F,0x1}, 
		{0,2,0xEB,0x1}, 
		{0,4,0x48,0x2}, 
		{0,5,0x15,0x2}, 
		{0,7,0x5F,0x1}, 
		{0,8,0xEB,0x1}, 
		{0,10,0x48,0x2}, 
		{0,11,0x15,0x2}, 
		{100,1,0x81,0x1}, 
		{0,2,0xFD,0x1}, 
		{0,4,0xFE,0x1}, 
		{0,5,0x4F,0x2}, 
		{0,7,0x58,0x1}, 
		{0,8,0xB1,0x1}, 
		{0,10,0x7F,0x2}, 
		{0,11,0x3,0x2}, 
		{100,3,0x7B,0x1}, 
		{0,4,0x38,0x2}, 
		{0,5,0xC8,0x1}, 
		{100,4,0xC9,0x2}, 
		{0,5,0x8B,0x1}, 
		{100,4,0xC,0x3}, 
		{0,5,0xC8,0x1}, 
		{100,3,0xCC,0x1}, 
		{0,4,0xA8,0x2}, 
		{0,5,0x4F,0x2}, 
		{100,6,0x85,0x2}, 
		{0,7,0xF4,0x0}, 
		{0,8,0x38,0x2}, 
		{100,7,0x37,0x1}, 
		{0,8,0x75,0x2}, 
		{100,7,0xC8,0x1}, 
		{0,8,0x38,0x2}, 
		{100,6,0x24,0x2}, 
		/*{100,6,0x34,0x2}, */
		{0,7,0x2,0x2}, 
		{0,8,0xB1,0x1}, 
		{100,1,0xB8,0x1}, 
		{0,2,0xEB,0x1}, 
		{0,4,0xA1,0x2}, 
		{0,5,0x15,0x2}, 
		{0,7,0xB8,0x1}, 
		{0,8,0xEB,0x1}, 
		{0,10,0xA1,0x2}, 
		{0,11,0x15,0x2}, 
		{100,1,0x2,0x2}, 
		{0,2,0xB1,0x1}, 
		{0,4,0x7F,0x2}, 
		{0,5,0x3,0x2}, 
		{0,7,0x81,0x1}, 
		{0,8,0xFD,0x1}, 
		{0,10,0xA8,0x2}, 
		{0,11,0x4F,0x2}};
		
const unsigned char turnRight1[108][4]={
		{100,0,0x34,0x2}, 
		{0,1,0x58,0x1}, 
		{0,2,0xB1,0x1}, 
		{0,3,0xCC,0x1}, 
		{0,4,0x7F,0x2}, 
		{0,5,0x3,0x2}, 
		{0,6,0x34,0x2}, 
		{0,7,0x81,0x1}, 
		{0,8,0xFD,0x1}, 
		{0,9,0xCC,0x1}, 
		{0,10,0xA8,0x2}, 
		{0,11,0x4F,0x2},
		{100,0,0x45,0x2},
		{0,1,0xE8,0x0},
		{0,2,0x44,0x2},
		{0,3,0xBB,0x1},
		{0,4,0xD1,0x2},
		{0,5,0x7E,0x1},
		{0,6,0x45,0x2},
		{0,7,0x2F,0x1},
		{0,8,0x82,0x2},
		{0,9,0xBB,0x1},
		{0,10,0x18,0x3},
		{0,11,0xBC,0x1},
		{100,0,0x50,0x2},
		{0,1,0x9,0x1},
		{0,2,0xCC,0x1},
		{0,3,0xF0,0x1},
		{0,4,0xA5,0x2},
		{0,5,0x53,0x1},
		{0,6,0x64,0x2},
		{0,7,0x25,0x1},
		{0,8,0x2E,0x2},
		{0,9,0xD4,0x1},
		{0,10,0x1C,0x3},
		{0,11,0x6A,0x1},
		{100,0,0x45,0x2},
		{0,1,0xA3,0x1},
		{0,2,0xD6,0x0},
		{0,3,0xF3,0x1},
		{0,4,0x8B,0x2},
		{0,5,0x8E,0x1},
		{0,6,0x58,0x2},
		{0,7,0x52,0x1},
		{0,8,0xEE,0x1},
		{0,9,0xDB,0x1},
		{0,10,0xEC,0x2},
		{0,11,0xA5,0x1},
		{100,9,0xD4,0x1},
		{0,10,0x1C,0x3},
		{0,11,0x6A,0x1},
		{100,9,0xBB,0x1},
		{0,10,0xD1,0x2},
		{0,11,0x7E,0x1},
		{100,9,0xC4,0x1},
		{0,10,0xAB,0x2},
		{0,11,0xB9,0x1},
		{100,0,0x50,0x2},
		{0,1,0x9,0x1},
		{0,2,0xCC,0x1},
		{100,0,0x45,0x2},
		{0,1,0x2F,0x1},
		{0,2,0x82,0x2},
		{100,0,0x3C,0x2},
		{0,1,0x55,0x1},
		{0,2,0x47,0x2},
		{100,3,0xF0,0x1},
		{0,4,0xA5,0x2},
		{0,5,0x53,0x1},
		{100,3,0xBB,0x1},
		{0,4,0x18,0x3},
		{0,5,0xBC,0x1},
		{100,3,0xC4,0x1},
		{0,4,0xE6,0x2},
		{0,5,0xF8,0x1},
		{100,6,0x64,0x2},
		{0,7,0x25,0x1},
		{0,8,0x2E,0x2},
		{100,6,0x45,0x2},
		{0,7,0xE8,0x0},
		{0,8,0x44,0x2},
		{100,6,0x3C,0x2},
		{0,7,0x1A,0x1},
		{0,8,0x8,0x2},
		{100,0,0x34,0x2},
		{0,1,0x81,0x1},
		{0,2,0xFD,0x1},
		{0,3,0xCC,0x1},
		{0,4,0xA8,0x2},
		{0,5,0x4F,0x2},
		{0,6,0x34,0x2},
		{0,7,0x58,0x1},
		{0,8,0xB1,0x1},
		{0,9,0xCC,0x1},
		{0,10,0x7F,0x2},
		{0,11,0x3,0x2},
		{100,0,0x34,0x2},
		{0,1,0x81,0x1},
		{0,2,0xFD,0x1},
		{0,3,0xCC,0x1},
		{0,4,0x7F,0x2},
		{0,5,0x3,0x2},
		{0,6,0x34,0x2},
		{0,7,0x81,0x1},
		{0,8,0xFD,0x1},
		{0,9,0xCC,0x1},
		{0,10,0x7F,0x2},
		{0,11,0x3,0x2}};

void spiralSearch(void) {
	
	// Scan!
	newPath = 0;
	
	// Enable ECAN1 Interrupt
	IEC2bits.C1IE = 1;
	C1INTEbits.TBIE = 1;
	C1INTEbits.RBIE = 1;
	IEC0bits.U1TXIE = 0;		// Disable UART1 Tx interrupt
	IEC0bits.U1RXIE = 0;		// Disable UART1 Rx interrupt
	_RE0 = 1;
	wait_ms(RS_485_DELAY);
	
	moveMotor(12,368);
	xPosition = 368;
	wait_ms(3000);
	
	/* sendHexAsASCII(rx_ecan1message.data[0]);
	sendHexAsASCII(rx_ecan1message.data[1]);
	sendHexAsASCII(rx_ecan1message.data[2]);
	sendHexAsASCII(rx_ecan1message.data[3]);
	sendHexAsASCII(rx_ecan1message.data[4]);
	sendHexAsASCII(rx_ecan1message.data[5]);
	sendHexAsASCII(rx_ecan1message.data[6]); */
	
	if(rx_ecan1message.data[6] == 0) {
	
		moveMotor(12,512);
		xPosition = 512;
		wait_ms(3000);
		if(rx_ecan1message.data[6] == 0) {
		
			moveMotor(12,655);
			xPosition = 655;
			wait_ms(3000);
			if(rx_ecan1message.data[6] == 0) {
				
				moveMotor(12,512);
				xPosition = 512;
				wait_ms(500);
				
				sendstring("Failed to see anything. \n\r");
				
				// Disable CAN interrupts
				IEC2bits.C1IE = 0;
				C1INTEbits.TBIE = 0;	
				C1INTEbits.RBIE = 0;
				IEC0bits.U1TXIE = 1; 		// Enable UART1 Tx interrupt
				IEC0bits.U1RXIE = 1; 		// Enable UART1 Rx interrupt
					
				// Next spiral element.
				if(toggle) {
					pathLength++;
					path = realloc(path,pathLength*sizeof(char));
					path[pathLength - 7] = 1;								// Take a step
					path[pathLength - 6] = 2;								// Turn right
					path[pathLength - 5] = 2;								// Turn right
					path[pathLength - 4] = 2;								// Turn right
					path[pathLength - 3] = 2;								// Turn right
					path[pathLength - 2] = 2;								// Turn right
					path[pathLength - 1] = 2;								// Turn right
					toggle = 0;
				}
				else {
					toggle = 1;
				}
				wait_ms(1000);
				//sendstring("WTF?");										// Taking this out breaks it for some reason, need to determine why...
				
				pathSwitch(path[0]);
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
				newPath = 0;
			}
			else if(rx_ecan1message.data[6] == 1) {
				sendstring("Almost saw something... Angle 3 \n\r");
				walkAStep();											// Walk forwards
			}
			else {
				sendstring("Saw something... Angle 3 \n\r");
				shootTarget();											// Target and shoot!
			}
		}
		else if(rx_ecan1message.data[6] == 1) {
			sendstring("Almost saw something... Angle 2 \n\r");
			walkAStep();												// Walk forwards
		}
		else {
			sendstring("Saw something... Angle 2 \n\r");
			shootTarget();												// Target and shoot!
		}
	}
	else if(rx_ecan1message.data[6] == 1) {
		sendstring("Almost saw something... Angle 1 \n\r");
		walkAStep();													// Walk forwards
	}
	else {
		sendstring("Saw something... Angle 1 \n\r");
		shootTarget();													// Target and shoot!
	}
	
	return;
}

void walkAStep(void) {
	// Disable CAN interrupts
	IEC2bits.C1IE = 0;
	C1INTEbits.TBIE = 0;	
	C1INTEbits.RBIE = 0;
	IEC0bits.U1TXIE = 1; 		// Enable UART1 Tx interrupt
	IEC0bits.U1RXIE = 1; 		// Enable UART1 Rx interrupt
	
	if(xPosition == 512) {
		pathLength = 2;
		path = realloc(path,pathLength*sizeof(unsigned char));
		path[0] = 1;
		path[1] = 0;
	}
	else if(xPosition == 368) {
		pathLength = 5;
		path = realloc(path,pathLength*sizeof(unsigned char));
		path[0] = 2;
		path[1] = 2;
		path[2] = 2;
		path[3] = 1;
		path[4] = 0;
	}
	else {
		pathLength = 5;
		path = realloc(path,pathLength*sizeof(unsigned char));
		path[0] = 2;
		path[1] = 2;
		path[2] = 2;
		path[3] = 1;
		path[4] = 0;
	}
	
	pathSwitch(path[0]);
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
	newPath = 0;
	
	return;
}

void shootTarget(void) {
	
	unsigned int xFail, yFail, increment;
	unsigned int test;
	
	xFail = 1;
	yFail = 1;
	
	IEC0bits.U1TXIE = 0; 		// Disable UART1 Tx interrupt
	IEC0bits.U1RXIE = 0; 		// Disable UART1 Rx interrupt
	_RE0 = 1;
	wait_ms(RS_485_DELAY);
	
	 increment = 5;
	while(xFail) {
		test = (((unsigned int)rx_ecan1message.data[0]) + (((unsigned int)rx_ecan1message.data[1]) << 8));
		if(test == 0) {
			wait_ms(2000);
		}
		else if(test < 80) {				// 175
			xPosition += increment;
			moveMotor(12,xPosition);
			wait_ms(2000);
		}
		else if(test > 90) {				// 185
			xPosition -= increment;
			moveMotor(12,xPosition);
			wait_ms(2000);
		}
		else
			xFail = 0;
	} 
	
	increment = 5;
	while(yFail) {
		test = (((unsigned int)rx_ecan1message.data[2]) + (((unsigned int)rx_ecan1message.data[3]) << 8));
		if(test == 0) {
			wait_ms(2000);
		}
		else if(test < 45) {				// 115
			yPosition += increment;
			moveMotor(13,yPosition);
			wait_ms(2000);
		}
		else if(test > 55) {				// 125
			yPosition -= increment;
			moveMotor(13,yPosition);
			wait_ms(2000);
		}
		else
			yFail = 0;
	}
	
	_RA4 = 1;		//FIRE!
	wait_ms(1000);
	_RA4 = 0;
	
	return;
}

void sonarScan(void) {
	sensorMode = 1;
	// Set up arrays for stuff to be stored in or used predetermined global var...
	I2C1CONbits.SEN = 1;		// Send start event.
	return;
}

void pathSwitch(unsigned char switchTest) {

	switch(switchTest) {
		case 0:
			nextCommand = stand;
			burstLengthSetup(12);
			break;
		case 1:
			nextCommand = walk;
			burstLengthSetup(84);
			break;
		case 2:
			nextCommand = turnRight1;
			burstLengthSetup(108);
			break;
		default: 
			while(1);
			break;
	}
	return;
}
