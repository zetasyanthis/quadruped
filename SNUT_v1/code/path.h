/*****************************************************************************************************
  *  Filename:			path.h
  *  Description:			Contains the function headers and global variables for path.c.
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

void spiralSearch(void);
void walkAStep(void);
void shootTarget(void);
void sonarScan(void);
void pathSwitch(unsigned char switchTest);

extern const unsigned char stand[12][4];
extern const unsigned char walk[84][4];
extern const unsigned char turnRight1[108][4];
extern unsigned int toggle;
extern unsigned int xPosition;
extern unsigned int yPosition;
