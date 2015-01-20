/*****************************************************************************************************
  *  Filename:			i2c.h
  *  Description:			Contains the function headers and global variables for i2c.c.
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

void initI2C(void);
void _ISR _MI2C1Interrupt(void);

extern char sensorState;
extern char sensorMode;
