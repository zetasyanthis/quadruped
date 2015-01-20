#include <glib.h>
#include <glib/gprintf.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sensors_ADC_init(void);
void sensors_accel_init(void);

FILE *fp;

float max_x_data, max_y_data, max_z_data = 0;

int
main(int argc, char** argv)
{
	sensors_ADC_init();
//	sensors_accel_init();
}

void
sensors_ADC_init(void) {
	int file;
	char filename[40];
	const gchar *buffer;
	int addr = 0b00101001;		// The I2C address of the ADC
	
	sprintf(filename,"/dev/i2c-2");
	if ((file = open(filename,O_RDWR)) < 0) {
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");

		exit(1);
	}
	
	if (ioctl(file,I2C_SLAVE,addr) < 0) {
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");
		exit(1);
	}
	
	char buf[10] = {0};
	float data;
	char channel;
	
	for(int i = 0; i<4; i++) {
		// Using I2C Read
		if (read(file,buf,2) != 2) {
			/* ERROR HANDLING: i2c transaction failed */
			printf("Failed to read from the i2c bus.\n");
			buffer = g_strerror(errno);
			printf(buffer);
			printf("\n\n");
		} else {
			data = (float)((buf[0] & 0b00001111)<<8)+buf[1];
			data = data/4096*5;
			channel = ((buf[0] & 0b00110000)>>4);
			printf("Channel %02d Data:  %04f\n",channel,data);
		}
	}

	//unsigned char reg = 0xFF; /* Device register to access */
	//buf[0] = reg;
	
	buf[0] = 0b11110000;
	if (write(file,buf,1) != 1) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to read from the i2c bus.\n");
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");		
	}
	close(file);
}

// For accelerometer
// Set FULL_RES bit in 0x31
// Read from 0x32 to 0x37

void
sensors_accel_init() {
	int file;
	char filename[40];
	const gchar *buffer;
	int addr = 0x53;		// The I2C address of the Accelerometer
	
	sprintf(filename,"/dev/i2c-2");
	if ((file = open(filename,O_RDWR)) < 0) {
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");

		exit(1);
	}
	
	if (ioctl(file,I2C_SLAVE,addr) < 0) {
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");
		exit(1);
	}
	
	char buf[10] = {0};
	
	buf[0] = 0x31;
	buf[1] = 0b00001000;
	if (write(file,buf,2) != 2) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to enable full_res mode.\n");
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");
	} else {
		printf("Enabled FULL_RES mode.\n");
	}
	
	buf[0] = 0x2D;
	buf[1] = 0b00001000;
	if (write(file,buf,2) != 2) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to enable measurement mode.\n");
		buffer = g_strerror(errno);
		printf(buffer);
		printf("\n\n");
	} else {
		printf("Enabled measurement mode.\n");
	}

while(1) {
	buf[0] = 0x32;
	if (write(file, buf, 1) != 1) {
		printf("Failed to swap internal address to the data buffer.\n");
	} else {
		printf("Successfully swapped internal address to the data buffer.\n");
	}
	
	if (read(file,buf,6) != 6) {
		printf("Failed to read from the x,y,z buffers.\n");
	} else {
		float x_data, y_data, z_data;
		x_data = 0.004 * (float)( buf[0] + ( buf[1] << 8 ));
		y_data = 0.004 * (float)( buf[2] + ( buf[3] << 8 ));
		z_data = 0.004 * (float)( buf[4] + ( buf[5] << 8 ));
		printf("%02x, %02x, %02x, %02x, %02x, %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[6]);
		printf("%03d, %03d, %03d, %03d, %03d, %03d\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[6]);
		printf("%2.3f, %2.3f, %2.3f\n", x_data, y_data, z_data);
		
		if(x_data > max_x_data) {
			max_x_data = x_data;
		}
		if(y_data > max_y_data) {
			max_y_data = y_data;
		}
		if(z_data > max_z_data) {
			max_z_data = z_data;
		}
		printf("%2.3f, %2.3f, %2.3f\n", max_x_data, max_y_data, max_z_data);
		sleep(1);
	}
}	
	close(file);
}

float
interpret_accel_data(unsigned char DATAx1, unsigned char DATAx0) {
	
	if((DATAx1 & 0b10000000) > 0) {
		switch(DATAx1 & 0b11111100) {
			case 0b11110000:
				
				break;
			case 0b11111000:
				
				break;
			case 0b11111100:
				
				break;
		}
	} else {
		return 0.004 * (float)( DATAx0 + ( DATAx1 << 8 ));
	}
	return 0;
}


