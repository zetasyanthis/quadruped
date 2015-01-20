/*****************************************************************************************************
  *  Filename:			i2c.c
  *  Description:			Contains the i2c initialization method and associated interrupt service routine.  
  *  Functions Contained:		initI2C, _MI2C1Interrupt
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#include "includes.h"

char sensorMode = 0;
char sensorState = 0;
char data[3];

void initI2C(void) {
	
	I2C1CONbits.I2CEN = 1;		// Enable I2C1 module
	I2C1CONbits.I2CSIDL = 0;	// Continue operation in idle mode
	//I2C1CONbits.SCLREL = ;			// Only for slave operation
	//I2C1CONbits.IPMIEN = ;			// Unknown?
	I2C1CONbits.A10M = 0;		// 7-bit addresses
	//I2C1CONbits.DISSLW = 1;			// Slew control disabled
	//I2C1CONbits.SMEN = 1;			// Enable SMBus I/O levels
	//I2C1CONbits.GCEN = ;			// Only for slave operation
	//I2C1CONbits.STREN = ;			// Only for slave operation
	
	I2C1BRG = 0x0BF;			// 400kHz operation from 80Mhz clock
	
	return;
}

void _ISR _MI2C1Interrupt(void) {
	
	if(sensorMode) {						// Sonar
		switch(sensorState) {
			case 0:
				// Idle mode.
				break;
			case 1:
				sensorState++;
				I2C1TRN = 0xE0;				// Address of sonar module
				break;
			case 2:
				sensorState++;
				I2C1TRN = 0x00;				// Command register
				break;
			case 3:
				sensorState++;
				I2C1TRN = 0x51;				// Tell the sonar to go to ranging mode, this will take 66ms to complete
				break;
			case 4:
				PR7 = (((unsigned int)313*(unsigned int)70) & 0xFFFF0000) >> 16;		// Set up timer to wait ~66 milliseconds.
				PR6 = (((unsigned int)313*(unsigned int)70) & 0x0000FFFF);				//
				T6CONbits.TON = 1;			// Start 32-bit Timer
				IEC3bits.T7IE = 1;			// Enable Timer7 interrupt because 32-bit mode wraps from T6 to T7
				sensorState++;
				I2C1CONbits.PEN = 1;		// Send stop event.
				break;
			case 5:
				// Data not ready yet.  Return and wait on timer.
				break;
			case 6:
				sensorState++;
				I2C1TRN = 0xE0;				// Address of sonar module
				break;
			case 7:
				sensorState++;
				I2C1TRN = 0x02;				// First echo register
				break;
			case 8:
				sensorState++;
				I2C1CONbits.RSEN = 1;		// Send repeated start event
				break;
			case 9:
				sensorState++;
				I2C1TRN = 0xE1;				// Address of sonar module with R/W high
				break;
			case 10:
				sensorState++;
				I2C1CONbits.RCEN = 1;		// Start receieve mode
				break;
			case 11:
				sensorState++;
				data[0] = I2C1RCV;		// Receive distance
				break;
			case 12:
				sensorState++;
				data[1] = I2C1RCV;		// Receive distance
				I2C1CONbits.ACKEN = 1;		// Send acknowledgement
				break;
			case 13:
				sensorState++;
				I2C1CONbits.PEN = 1;		// Send stop event.
				break;
			case 14:
				sensorState = 1;
				I2C1CONbits.SEN = 1;		// Send start event.
				break;
		}
	}
	else {									// Accelerometer
		switch(sensorState) {
			case 0:
				// Idle mode.
				break;
			case 1:
				sensorState++;
				I2C1TRN = 0b00111000;		// Address of accel  module...  ASSUMES SD0 is connected to GROUND
				break;
			case 2:
				sensorState++;
				I2C1TRN = 0x29;				// First echo register
				break;
			case 3:
				sensorState++;
				I2C1CONbits.RSEN = 1;		// Send repeated start event
				break;
			case 4:
				sensorState++;
				I2C1TRN = 0b00111001;		// Address of accel module with R/W high
				break;
			case 5:
				sensorState++;
				I2C1CONbits.RCEN = 1;		// Start receieve mode
				break;
			case 6:
				sensorState++;
				data[0] = I2C1RCV;			// Receive distance
				I2C1CONbits.ACKEN = 1;		// Send acknowledgement
				break;
			case 7:
				sensorState++;
				data[1] = I2C1RCV;			// Receive distance
				I2C1CONbits.ACKEN = 1;		// Send acknowledgement
				break;
			case 8:
				sensorState++;
				data[1] = I2C1RCV;			// Receive distance
				I2C1CONbits.ACKEN = 1;		// Send acknowledgement
				break;
			case 9:
				sensorState++;
				data[2] = I2C1RCV;			// Receive distance
				I2C1CONbits.ACKEN = 1;		// Send acknowledgement
				break;
			case 10:
				sensorState++;
				data[2] = I2C1RCV;			// Receive distance
				I2C1CONbits.ACKEN = 1;		// Send acknowledgement
				break;
			case 11:
				sensorState = 0;
				I2C1CONbits.PEN = 1;		// Send stop event.
				break;
		}
	}
	return;
}
