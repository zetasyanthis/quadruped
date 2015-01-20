/*****************************************************************************************************
  *  Filename:			a2d.c
  *  Description:			Contains the A/D initialization method and associated interrupt service routine.  
  *					This services the IR sensors and stops to rebuild the path if they pass a threshold.  
  *  Functions Contained:		initAD, _ADC1Interrupt
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

#include "includes.h"

//=================================================================================
// initAD
//
//		Initializes A/D for 2 channel mode.  AN3 is CH0, AN0 is CH1
//
//=================================================================================
void initAD(void)
{
	AD1CON1bits.ADON = 1;		// Turn A/D On.
	AD1CON1bits.ADSIDL = 0;		// Continue operation in idle mode.
	AD1CON1bits.AD12B = 0;		// 10-bit, up to 4 channel operation.
	AD1CON1bits.FORM = 0b00;	// Integer output.
	AD1CON1bits.SSRC = 0b010;	// GP timer compare ends sampling and starts conversion.
	AD1CON1bits.SIMSAM = 0b01;	// Samples CH0 and CH1 simultaneously
	AD1CON1bits.ASAM = 1;		// Sampling begins immediately after last conversion.
	//AD1CON1bits.SAMP = 1;			// Autoset by hardware.
	
	AD1CON2bits.VCFG = 0b000;	// Avdd is Vref+, Avss is Vref-
	AD1CON2bits.CSCNA = 0;		// Do not scan inputs
	AD1CON2bits.CHPS = 0b01;	// Convert CH0 and CH1
	AD1CON2bits.SMPI = 0b001;	// Interrupts at the completion of every 2nd sample/convert sequence
	AD1CON2bits.BUFM = 0;		// Always starts filling the buffer from the start address.
	AD1CON2bits.ALTS = 1;		// MUXA and MUXB control bits alternatively select the analog input for conversion
	
	AD1CON3bits.ADRC = 0;		// Clock derived from system clock
	//AD1CON3bits.SAMC = ;
	//AD1CON3bits.ADCS = ;
	
	// Initialize MUXA Input Selection
	AD1CHS0bits.CH0SA = 3;		// Select AN3 for CH0 +ve input
	AD1CHS0bits.CH0NA = 0;		// Select VREF- for CH0 -ve input
	AD1CHS123bits.CH123SA=0;	// Select AN0 for CH1 +ve input
								// Select AN1 for CH2+ve input
								// Select AN2 for CH3 +ve input
	AD1CHS123bits.CH123NA=0;	// Select VREF- for CH1/CH2/CH3 -ve inputs
	
	AD1PCFGL = 0b1001;			// Set pins AN0 and AN3 to be analog inputs

	IPC3bits.AD1IP = 0x01;		// Interrupt priority 1
	AD1CON1bits.DONE = 0;		// Clears conversion cycle complete bit.
	IFS0bits.AD1IF = 0;			// Clear A/D interrupt flag
	IEC0bits.AD1IE = 1;			// Enable A/D interrupt
	
	return;
}

void _ISR _ADC1Interrupt(void) {
	
	IFS0bits.AD1IF = 0;		// Clear A/D interrupt flag
	AD1CON1bits.DONE = 0;	// Clears conversion cycle complete bit.
	
	// Do stuff
	
	return;
}
