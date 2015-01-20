/*****************************************************************************************************
  *  Filename:			helper.h
  *  Description:			Contains the function headers and global variables for helper.c.
  *  Author:				Mark Jacobson
  *  InitDate: 			Sept. 8, 2008
  *  ModDate:			Nov. 8, 2008
  *  Liscence:			N/A
  *****************************************************************************************************/

//  Motor Instructions
#define PING 0x01
#define READ_DATA 0x02
#define WRITE_DATA 0x03
#define WRITE_REG 0x04
#define ACTION 0x05
#define RESET 0x06
#define SYNC 0x83
#define BROADCAST 0xFE

//  Other Parameters
#define SEND_TRIES 3
#define CENTER 512
#define ANGLE(x)

void wait_ms(float delay_ms);
void criticalError(void);
void burstLengthSetup(int totalLength);
char addCheckSum(unsigned char* byteArray);
int abs(int number);
int moveMotor(unsigned char motorID,int position);
int sendPacket(unsigned char motorID, char instrID, char numParams, char* params);
void sendCommand(char* command);
