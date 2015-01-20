/*****************************************************************************************************
  *  Filename:			timers.c
  *  Description:			Contains the timer initialization method and associated interrupt service routines.  
  *  Functions Contained:		initTimers, _T2Interrupt, _T4Interrupt, _T6Interrupt
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#include "includes.h"

unsigned char sonarMotorMoved = 0;

//=================================================================================
// initTimers
//
//		Set up timers for 32 bit mode
//
//=================================================================================
void initTimers(void)
{
	/*// Timer 2-3
	T2CONbits.TSIDL=0;		// Continue operation in idle mode.
	T2CONbits.TGATE=0;		// Disable gated time accumulation.
	T2CONbits.TCKPS=0b11;	// Set prescaler to 256.
	T2CONbits.T32=1;		// 32 bit mode
	T2CONbits.TCS=0;		// Source = internal clock;
	IPC2bits.T3IP = 0x01;	//Set interrupt priority
	IFS0bits.T3IF = 0;		// Clear Timer3 interrupt flag
	PR3 = (((unsigned int)313*(unsigned int)125) & 0xFFFF0000) >> 16;		// Set up timer to wait 125 milliseconds.
	PR2 = (((unsigned int)313*(unsigned int)125) & 0x0000FFFF);				//
	*/
	// Timer 4-5
	T4CONbits.TSIDL=0;		// Continue operation in idle mode.
	T4CONbits.TGATE=0;		// Disable gated time accumulation.
	T4CONbits.TCKPS=0b11;	// Set prescaler to 256.
	T4CONbits.T32=1;		// 32 bit mode
	T4CONbits.TCS=0;		// Source = internal clock;
	IPC7bits.T5IP = 0x01;	//Set interrupt priority
	IFS1bits.T5IF = 0;		// Clear Timer5 interrupt flag
	//IEC1bits.T5IE = 0;			// Enable Timer5 interrupt because 32-bit mode wraps from T2 to T3
	/*
	// Timer 6-7
	T6CONbits.TSIDL=0;		// Continue operation in idle mode.
	T6CONbits.TGATE=0;		// Disable gated time accumulation.
	T6CONbits.TCKPS=0b11;	// Set prescaler to 256.
	T6CONbits.T32=1;		// 32 bit mode
	T6CONbits.TCS=0;		// Source = internal clock;
	IPC12bits.T7IP = 0x01;	//Set interrupt priority
	IFS3bits.T7IF = 0;		// Clear Timer7 interrupt flag
	//IEC3bits.T7IE = 0;			// Enable Timer7 interrupt because 32-bit mode wraps from T2 to T3
	PR7 = (((unsigned int)313*(unsigned int)66) & 0xFFFF0000) >> 16;		// Set up timer to wait 66 milliseconds.
	PR6 = (((unsigned int)313*(unsigned int)66) & 0x0000FFFF);				//
	*/
	return;
}

//=================================================================================
// _T2Interrupt
//
//		Handles the timer2 interrupt.
//		Signal to trigger A/D autosent, this is a placeholder for now.
//
//=================================================================================
void _ISR _T3Interrupt(void) {
	IFS0bits.T3IF = 0;	// Clear Timer3 interrupt flag
	T4CONbits.TON = 0;	// Stop 32-bit Timer
	IEC0bits.T3IE = 0;	// Disable Timer3 interrupt because 32-bit mode wraps from T2 to T3
	
	// Send request to accelerometer
	//sensorMode = 0;
	sensorState = 1;
	I2C1CONbits.SEN = 1;	// Send start event
	
	return;
}

//=================================================================================
// _T4Interrupt
//
//		Handles the timer2 interrupt.
//		Triggers the next command to be run
//
//=================================================================================
void _ISR _T5Interrupt(void) {
	IFS1bits.T5IF = 0;	// Clear Timer3 interrupt flag
	IEC1bits.T5IE = 0;	// Disable Timer5 interrupt because 32-bit mode wraps from T4 to T5

	timerCycles++;
	T4CONbits.TON = 0;	// Stop 32-bit Timer
	
	//sendHexAsASCII(0xAA);
	
	U1TXREG = commandBuffer[0];
	
	return;
}

//=================================================================================
// _T6Interrupt
//
//		Handles the timer6 interrupt.
//		Triggers the next command to be run
//
//=================================================================================
void _ISR _T7Interrupt(void) {
	IFS3bits.T7IF = 0;	// Clear Timer7 interrupt flag
	T6CONbits.TON = 0;	// Stop 32-bit Timer
	IEC3bits.T7IE = 0;	// Disable Timer7 interrupt because 32-bit mode wraps from T6 to T7

	if(!sonarMotorMoved) {
		// Set up one motor movement and poll and send the first byte.  
		// The first set of array values must be set up on the fly for the incremental movement to work...
		// Do not modify the global values or at least reset them to what they were!
		// Trigger this timer fairly often when the motor is moving to make sure we start the sonar asap
		PR7 = (((unsigned int)313*(unsigned int)2) & 0xFFFF0000) >> 16;			// Set up timer to wait 66 milliseconds.
		PR6 = (((unsigned int)313*(unsigned int)2) & 0x0000FFFF);				//
	}
	else if(newPath) {
		newPath = 0;
		sensorState++;
		I2C1CONbits.SEN = 1;	// Send start event
	}
	
	return;
}
