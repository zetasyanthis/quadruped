/*****************************************************************************************************
  *  Filename:			timers.h
  *  Description:			Contains the function headers and global variables for timers.c.
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

void initTimers(void);
void _ISR _T2Interrupt(void);
void _ISR _T4Interrupt(void);
void _ISR _T6Interrupt(void);
