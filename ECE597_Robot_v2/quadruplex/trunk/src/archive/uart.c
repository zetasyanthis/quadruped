/*****************************************************************************************************
  *  Filename:			uart.c
  *  Description:			Contains the uart initialization method and associated interrupt service routines.  
  *					The RX interrupt contains the state machine that drives the rs-485 packet sending.  
  *					Also contains legacy code from v1.
  *  Functions Contained:		initUART, sendByte, getByte, sendHexAsASCII, sendString, _U1TXInterrupt, 
  *					_U1RXInterrupt
  *  Author:				Adam Hoffman, Mark Jacobson
  *  InitDate: 			May. 5, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#include "includes.h"

unsigned char bufferSwap = 1;
unsigned char speedMode;
int motorPosition[12][2];
unsigned char* nextCommand;
unsigned char speedBuffer[12][3];
unsigned char motorsUsed[12];
unsigned char speedChanges;
unsigned char newComplex;
unsigned int newSpeed;
unsigned char polling = 0;

//=================================================================================
// initUART(void)
//
//		Initializes both UART1 and UART2
//
//=================================================================================
void initUART(void)
{
	//set up UART2 (serial terminal)
	U2MODEbits.PDSEL = 0; 		// No Parity, 8-data bits
	U2MODEbits.ABAUD = 0; 		// Autobaud Disabled
	U2MODEbits.BRGH = 1; 		// High Speed mode
	SET_BAUD_RATE2(115200);		// Set BAUD Rate
	U2STAbits.UTXISEL0 = 0; 	// Interrupt after one Tx character is transmitted
	U2STAbits.UTXISEL1 = 0;
	U2STAbits.UTXINV = 0;		// Don't invert transmit polarity
	U2MODEbits.URXINV = 0;		// Don't invert receive polarity
	U2MODEbits.UEN = 0b00;		// UxTX and UxRX pins are enabled and used.  
	
	//set up UART1 (motors)
	U1MODEbits.PDSEL = 0; 		// No Parity, 8-data bits
	U1MODEbits.ABAUD = 0; 		// Autobaud Disabled
	U1MODEbits.BRGH = 0; 		// High Speed mode
	U1MODEbits.USIDL = 0;		// Continue in idle mode.
	U1MODEbits.IREN = 0;		// Disable IrDA encoding.
	U1MODEbits.RTSMD = 0;		// UxRTS pin in flow control mode.
	U1MODEbits.UEN = 0b00;		// UxTX and UxRX pins are enabled and used.  
	
	SET_BAUD_RATE1(250000);		// Set BAUD Rate
	U1STAbits.UTXISEL1 = 1; 	// Interrupt when a character is transferred to the Transmit Shift Register, and as a result, the transmit buffer becomes empty
	U1STAbits.UTXISEL0 = 0;		//
	U1STAbits.URXISEL1 = 0;		// Interrupt is set when any character is received and transferred from the UxRSR to the receive buffer. 
	U1STAbits.UTXISEL0 = 0;		// Receive buffer has one or more characters.
	U1STAbits.UTXINV = 1;		// Invert transmit polarity
	U1MODEbits.URXINV = 1;		// Invert receive polarity

	//enable UARTs and UART TXs
	U1MODEbits.UARTEN = 1; 		// Enable UART1
	U2MODEbits.UARTEN = 1; 		// Enable UART2
	U1STAbits.UTXEN = 1; 		// Enable UART1 Tx
	U2STAbits.UTXEN = 1; 		// Enable UART2 Tx
	
	IPC7bits.U2RXIP = 0x07;		// Set recieve interrupt priority
	IPC3bits.U1TXIP = 0x01;		// Set recieve interrupt priority
	IFS0bits.U1TXIF = 0;		// Clear transmit interrupt flag
	IFS1bits.U2RXIF = 0;		// Clear receive interrupt flag

	IEC0bits.U1TXIE = 1; 		// Enable UART1 Tx interrupt
	IEC0bits.U1RXIE = 1; 		// Enable UART1 Rx interrupt

	return;
}

//=================================================================================
// sendByte(char byte, char channel)
//
//		Sends a byte of data to the serial port on USART channel 1 or 2
//
//=================================================================================
void sendByte(char byte, char channel)
{
//	int i;
//	_RF0 = 1; 						// Set direction to transmit
//	for(i=0;i<150;i++){}			// Wait for pin to change state
	
	if(channel==2)
	{
		while(U2STAbits.UTXBF);		// Wait for buffer to be empty
		U2TXREG = byte; 			// Transmit one character
	}
	else
	{
		while(U1STAbits.UTXBF);		// Wait for buffer to be empty
		U1TXREG = byte; 			// Transmit one character
	}
	return;
}


//=================================================================================
// getByte(char channel)
//
//		Gets a byte of data from the serial port on USART channel 1 or 2
//
//=================================================================================
char getByte(char channel)
{
	char byte;
	_RF0 = 0; 					// Set direction to receive and wait a short while
	if(channel==2)
	{
		while(!(U2STAbits.URXDA));	// Wait for receive buffer to have data
		byte = U2RXREG; 			// Receive one character
	}
	else
	{
		while(!(U1STAbits.URXDA));	// Wait for receive buffer to have data
		byte = U1RXREG; 			// Receive one character
	}
	return byte;
}


//=================================================================================
// sendHexAsASCII(char byte)
//
//		Sends ASCII characters out to a terminal on the serial port.  Not needed
//		unless debugging.
//
//=================================================================================
void sendHexAsASCII(char byte)
{
//	int i;
	char byteH = ((byte & 0xF0) >> 4);	// Get high half-byte
	char byteL = (byte & 0x0F);			// Get low half-byte
	char ASCII[] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
					0x41,0x42,0x43,0x44,0x45,0x46};  //ASCII codes for 0 - F
//	_RF0 = 0; 					// Set direction to transmit and wait a short while
//	for(i=0;i<550;i++){};		// Wait for pin to change state
	sendByte(ASCII[byteH],2); 	// Transmit ASCII character
	sendByte(ASCII[byteL],2);	// Transmit ASCII character
	sendByte(0x20,2);			// Transmit space character
	return;
}

//=================================================================================
// sendString(char* string, char size)
//
//		Sends a string of data to the serial port on USART channel 2 (terminal)
//
//=================================================================================
void sendString(char* string, char size)
{
	char i;
	for(i=0;i<size;i++)
	{
		while(U2STAbits.UTXBF);		// Wait for buffer to be empty
		U2TXREG = string[i]; 		// Transmit one character
		while(!(U2STA & 0x0100));
	}
	return;
}

//=================================================================================
// _U1TXInterrupt
//
//		Handles the uart 1 transmit interrupt, which triggers on transmission of a byte.
//		Checks to see if the buffer to be sent is empty and sends the next portion if not, else it returns.
//
//=================================================================================
void _ISR _U1TXInterrupt(void) {

	IFS0bits.U1TXIF = 0;									// Clear interrupt flag
	
	byteNum++;
	
	if(byteNum >= (commandLength))
	{
		byteNum = 0;

		// Set up recv
		//if(commandBuffer[2] == READ_DATA)
		//	returnLength = PACKET_NO_PARAMS + 2;									// Default status packet + number of parameters
		//else
			returnLength = PACKET_NO_PARAMS;										// Default status packet
			
		wait_ms(RS_485_DELAY);
		_RE0 = 0;																	// Set recieve mode.
		
		return;
	}
	
	U1TXREG = commandBuffer[byteNum];
	
	//DEBUG
	//sendHexAsASCII(commandBuffer[byteNum]);													// Removing this seems to break it?

	return;
}

//=================================================================================
// _U1RXInterrupt
//
//		Handles the UART1 receive interrupt, which triggers on reception of a byte.
//		Checks to see what piece of the return packet it is receiving and stores it there.
//
//=================================================================================
void _ISR _U1RXInterrupt(void) {
	int i;
	unsigned char* temp;
	
	IFS0bits.U1RXIF = 0;									// Clear interrupt flag
	returnPacket[byteNum] = U1RXREG;						// Store latest byte of return packet
	byteNum++;
	
	if(byteNum >= (returnLength))
	{
		if((returnPacket[4])) {									// If an error occurs, freeze!
			error = returnPacket[4];
			for(i=0;i<commandLength;i++)
				sendHexAsASCII(commandBuffer[i]);
			sendstring("\n\r");
			for(i=0;i<returnLength;i++)
				sendHexAsASCII(returnPacket[i]);
			sendstring("\n\rFraming Error:  ");
			sendHexAsASCII(U1STAbits.FERR);
			criticalError();
		}
		
		byteNum = 0;
		_RE0 = 1;											// Set transmit mode.
		wait_ms(RS_485_DELAY);
		commandNum++;
		
		if(speedMode && (commandNum < speedChanges)) {
			
			currentCommand += 3;				// Advance to next command in burst and assemble it for transmission
			
			commandBuffer[2] = *(currentCommand);					// motor ID
			commandBuffer[3] = 5;									// Length = instruction + address + 2 byte velocity + checksum = 5
			commandBuffer[4] = WRITE_DATA;							// Instruction
			commandBuffer[5] = 0x20;								// Motor velocity address
			commandBuffer[6] = *(currentCommand + 1);				// Low byte of velocity
			commandBuffer[7] = *(currentCommand + 2);				// High byte of velocity
			commandBuffer[8] = addCheckSum(commandBuffer);			// Checksum
			
			commandLength = 9;
			returnLength = PACKET_NO_PARAMS;						// 6
			
			U1TXREG = commandBuffer[byteNum];						// Begin command transmission
			return;
		}
		if((commandNum >= burstLengths[burstNum]) || polling || speedMode) {			// If burst complete...
			commandNum = 0;
			//sendHexAsASCII(burstNum);
			if((burstNum >= (complexLength-1)) && !polling && !speedMode) {				// If complex command complete...
				//return;		// Use this if not complex switching!
				
				burstNum = 0;
				complexNum++;
				if(complexNum >= pathLength) {											// If path complete...
					complexNum = 0;
					newPath = 1;
					return;
				}
				
				pathSwitch(path[complexNum]);
				sendstring("New Complex\n\r");
				
				// Swap buffers and copy new motor positions there.  
				bufferSwap = !bufferSwap;
				for(i=0;i<12;i++) {
					motorPosition[i][bufferSwap] = motorPosition[i][!bufferSwap];
					motorsUsed[i] = i;
				}
				temp = nextCommand;
				for(i=0;i<burstLengths[0];i++) {
					motorPosition[nextCommand[1]][bufferSwap] = (nextCommand[3] << 8) + nextCommand[2];
					nextCommand += STATIC_COMMAND_LENGTH;
				}
				nextCommand = temp;
				newComplex = 1;			// Set flag so we don't overwrite the motor positions later
				speedChanges = 12;
			}
			
			if(polling || speedMode) {						// If we're polling already.
				if((returnPacket[5] != 0) || (timerCycles == 1) || speedMode) {			//Test for poll completion
					polling = 0;
					if(!speedMode) {
						speedMode = 1;
						if(!newComplex) {
							bufferSwap = !bufferSwap;
							nextCommand = currentCommand + STATIC_COMMAND_LENGTH;
							speedChanges = burstLengths[burstNum + 1];			// Save for later
							for(i=0;i<speedChanges;i++) {
								currentCommand += STATIC_COMMAND_LENGTH;
								motorPosition[currentCommand[1]][bufferSwap] = (currentCommand[3] << 8) + currentCommand[2];
								motorsUsed[i] = currentCommand[1];
							}
						}
						// Calc new motor speeds and save pointer
						for(i=0;i<speedChanges;i++) {
							speedBuffer[i][0] = motorsUsed[i];
							newSpeed = (int)(((float)abs(motorPosition[motorsUsed[i]][bufferSwap] - motorPosition[motorsUsed[i]][!bufferSwap]))*(float)900/((float)(duration[burstNum])*(float)10));
							// 835.23
							speedBuffer[i][1] = (unsigned char) (newSpeed & 0x00FF);
							speedBuffer[i][2] = (unsigned char) ((newSpeed & 0xFF00) >> 8);
							/* sendHexAsASCII((motorPosition[motorsUsed[i]][bufferSwap] & 0xFF00) >> 8);
							sendHexAsASCII((motorPosition[motorsUsed[i]][bufferSwap] & 0x00FF));
							sendHexAsASCII((motorPosition[motorsUsed[i]][!bufferSwap] & 0xFF00) >> 8);
							sendHexAsASCII((motorPosition[motorsUsed[i]][!bufferSwap] & 0x00FF));
							sendHexAsASCII((abs(motorPosition[motorsUsed[i]][bufferSwap] - motorPosition[motorsUsed[i]][!bufferSwap]) & 0xFF00) >> 8);
							sendHexAsASCII((abs(motorPosition[motorsUsed[i]][bufferSwap] - motorPosition[motorsUsed[i]][!bufferSwap]) & 0x00FF));
							sendHexAsASCII(speedBuffer[i][2]);
							sendHexAsASCII(speedBuffer[i][1]);
							sendstring("\n\r"); */
							// speedBuffer[i][1] = (unsigned char) (400 & 0x00FF);
							// speedBuffer[i][2] = (unsigned char) ((400 & 0xFF00) >> 8);
						}
						wait_ms(200);
						
						currentCommand = speedBuffer;
						
						commandBuffer[2] = *(currentCommand);					// motor ID
						commandBuffer[3] = 5;									// Length = instruction + address + 2 byte velocity + checksum = 5
						commandBuffer[4] = WRITE_DATA;							// Instruction
						commandBuffer[5] = 0x20;								// Motor velocity address
						commandBuffer[6] = *(currentCommand + 1);				// Low byte of velocity
						commandBuffer[7] = *(currentCommand + 2);				// High byte of velocity
						commandBuffer[8] = addCheckSum(commandBuffer);			// Checksum
						
						commandLength = 9;
						returnLength = PACKET_NO_PARAMS;						// 6
						
						U1TXREG = commandBuffer[byteNum];						// Begin command transmission
					}
					else {														// Exit speed mode
						
						speedMode = 0;
						currentCommand = nextCommand;
						commandNum = 0;
						if(newComplex)
							newComplex = 0;
						else
							burstNum++;
						
						commandBuffer[2] = *(currentCommand+1);					// motor ID
						commandBuffer[3] = 5;									// Length = instruction + address + 2 byte angle + checksum = 5
						commandBuffer[4] = WRITE_DATA;							// Instruction
						commandBuffer[5] = 0x1E;								// Motor position address
						commandBuffer[6] = *(currentCommand+2);					// Low byte of angle
						commandBuffer[7] = *(currentCommand+3);					// High byte of angle
						commandBuffer[8] = addCheckSum(commandBuffer);			// Checksum
							
						commandLength = 9;
						returnLength = PACKET_NO_PARAMS;						// 6
					
						U1TXREG = commandBuffer[byteNum];							// Begin command transmission
						return;
					}
				}
				else {		// If we've not elapsed and the motor is still moving.
					//sendHexAsASCII(0xBF);
					//Set up timer.
					time = (unsigned long)((float)duration[burstNum]*(float)313*(float)5)/(float)20;	//*0.80;			// (40000000/256) /1000 * (number of 10s of ms) = amount of time the timer has to run]
					//time = ((unsigned int)313*(unsigned int)duration[burstNum]*(unsigned int)20)/(unsigned int)20; //0.05;			// (40000000/256) /1000 * (number of 10s of ms) = amount of time the timer has to run
					//sendHexAsASCII(duration[burstNum]);
					//sendHexAsASCII(((time & 0xFF000000) >> 24));
					//sendHexAsASCII(((time & 0x00FF0000) >> 16));
					//sendHexAsASCII(((time & 0x0000FF00) >> 8));
					//sendHexAsASCII(((time & 0x000000FF)));
					
					PR5 = ((time & 0xFFFF0000) >> 16);				// Set up timer to wait x milliseconds.
					PR4 = (time & 0x0000FFFF);						//
					//wait_ms(0.1);
					IEC1bits.T5IE = 1;								// Ensable Timer5 interrupt because 32-bit mode wraps from T4 to T5
					T4CONbits.TON = 1;								// Start 32-bit Timer
					return;
				}
			}
			else {														// Set up poll packet
				commandBuffer[2] = *(currentCommand+1);					// ID
				commandBuffer[3] = 4;									// Length = instruction + start address + length + checksum = 4
				commandBuffer[4] = READ_DATA;							// Instruction
				commandBuffer[5] = 0x2E;								// Starting address (Moving)
				commandBuffer[6] = 1;									// Number of bytes to return
				commandBuffer[7] = addCheckSum(commandBuffer);			// Checksum
				
				timerCycles = 0;
				commandLength = 8;
				returnLength = PACKET_NO_PARAMS + 1;					// Poll return size
				//burstLengths[burstNum] = 1;
				polling = 1;
				
				// Set up timer.
				time = (unsigned long)((float)duration[burstNum]*(float)313*(float)5*(float)20)/(float)20;	//*0.80;			// (40000000/256) /1000 * (number of 10s of ms) = amount of time the timer has to run]
				//time = ((unsigned int)313*(unsigned int)duration[burstNum]*(unsigned int)500*(unsigned int)16)/(unsigned int)20;	//*0.80;			// (40000000/256) /1000 * (number of 10s of ms) = amount of time the timer has to run]
				/* sendHexAsASCII(duration[burstNum]);
				sendHexAsASCII(((time & 0xFF000000) >> 24));
				sendHexAsASCII(((time & 0x00FF0000) >> 16));
				sendHexAsASCII(((time & 0x0000FF00) >> 8));
				sendHexAsASCII(((time & 0x000000FF)));*/
				 
				PR5 = ((time & 0xFFFF0000) >> 16);				// Set up timer to wait x milliseconds.
				PR4 = (time & 0x0000FFFF);						//
				IEC1bits.T5IE = 1;								// Enable Timer5 interrupt because 32-bit mode wraps from T4 to T5
				T4CONbits.TON = 1;								// Start 32-bit Timer
				wait_ms(0.1);
				return;
			}
		}
		else {														// Advance to next command in burst and assemble it for transmission

			currentCommand += STATIC_COMMAND_LENGTH;

			commandBuffer[2] = *(currentCommand+1);					// motor ID
			commandBuffer[3] = 5;									// Length = instruction + address + 2 byte angle + checksum = 5
			commandBuffer[4] = WRITE_DATA;							// Instruction
			commandBuffer[5] = 0x1E;								// Motor position address
			commandBuffer[6] = *(currentCommand+2);					// Low byte of angle
			commandBuffer[7] = *(currentCommand+3);					// High byte of angle
			commandBuffer[8] = addCheckSum(commandBuffer);			// Checksum
				
			commandLength = 9;
			returnLength = PACKET_NO_PARAMS;						// 6
			
			U1TXREG = commandBuffer[byteNum];						// Begin command transmission
		}
	}
	
	return;
}
