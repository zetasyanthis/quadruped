#include <glib.h>
#include <unistd.h>
#include <string.h>

#include <sensors/i2c_bus.h>
#include <sensors/adxl345.h>

/** Constructor for ADXL345 struct
* \param bus I2C bus to associate the device with.
* \param 
*/
ADXL345 *
adxl345_new(I2CBus *bus, guint8 i2c_addr, uint8 bw_rate, uint8 data_format)
{
	ADXL345 *accel = g_new0(ADXL345, 1);

	accel->i2c_addr = i2c_addr;
	accel->mutex = g_mutex_new();

	accel->x_offset = DEFAULT_X_OFFSET;
	accel->y_offset = DEFAULT_Y_OFFSET;
	accel->z_offset = DEFAULT_Z_OFFSET;
	accel->data_rate = DEFAULT_DATA_RATE;
	accel->data_format = DEFAULT_DATA_FORMAT;
	accel->justify = DEFAULT_JUSTIFY;

	i2c_bus_add_driver(bus, i2c_driver, accel);

	return accel;
}



/** Sets the data format output by the ADXL345
* \param range Sets the g range the device is capable of sensing.  Valid arguments are 2,4,8,16, and FULL_RES.
* \param justify Sets LEFT or RIGHT justify. 
*/
static gboolean
accel_set_data_format(I2CBus *bus, ADXL345* accel, guint range, guint justify, gboolean report_errors)
{
	guint8 register = 0;
	
	switch(range) {
		case 2:
			register |= 0b00000000;
			break;
		case 4:
			register |= 0b00000001;
			break;
		case 8:
			register |= 0b00000010;
			break;
		case 16:
			register |= 0b00000011;
			break;
		case 0:	
			register |= 0b00001000;
			g_print("Set data format to full resolution mode ");
			break;
		default:
			g_error("Not a valid range.");

	}

	if(justify == LEFT)
	{
		register |= 0b00000010;
	}
	
	if (!i2c_bus_write_byte(bus, accel->i2c_addr, ADXL345_DATA_FORMAT_REG, register, report_errors)) {
		return FALSE;
	}

	accel->data_format = register;

	return TRUE;
}



/**	Sets offset values on the ADXL345 to account for device drift
*	
*/
static gboolean
accel_set_offsets(I2CBus *bus, ADXL345* accel, gfloat x_offset, gfloat y_offset, gfloat z_offset, gboolean report_errors)
{

	//	DUMMY FUNCTION

	return TRUE;
}




/**	Sets device sampling rate
*	
*/
static gboolean
accel_set_bw_rate(I2CBus *bus, ADXL345* accel, guint8 frequency, gboolean report_errors)
{
	
	//	DUMMY FUNCTION

	return TRUE;
}



static gboolean
accel_update_data(I2CBus *bus, ADXL345* accel, gboolean report_errors)
{
	g_print("\x1b[21AUpdating data\n");
	g_get_current_time(&accel->timestamp);

	guint8 buffer[6] = {0};

	if(!i2c_bus_read_buffer(bus, accel->i2c_addr, 0x32, 6, buffer, report_errors)) {
		return FALSE;
	} else {
		if (accel->justify == LEFT)
		{
			switch(accel->data_format) {
				case 2:
					break;
				case 4:
					break;
				case 8:
					break;
				case 16:
					break;
				case 0:
					break;
				default:
					g_error("Not a valid range.");
			}
		} else if (accel->justify == RIGHT) {
			switch(accel->data_format) {
				case 2:
					break;
				case 4:
					break;
				case 8:
					break;
				case 16:
					break;
				case 0:
					break;
				default:
					g_error("Not a valid range.");
			}
		} else {
			g_error("Stored justify value has become corrupted.");
		}
	}
	
	return TRUE;
}


