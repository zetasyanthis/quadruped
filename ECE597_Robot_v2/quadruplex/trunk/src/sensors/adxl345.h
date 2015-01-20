
// Device Address
#define ADXL345_ADDR 0x53

// For data format setting
#define FULL_RES 0
#define LEFT 1
#define RIGHT 0

// Default register values
#define DEFAULT_X_OFFSET 0
#define DEFAULT_Y_OFFSET 0
#define DEFAULT_Z_OFFSET 0
#define DEFAULT_DATA_RATE 100
#define DEFAUlT_JUSTIFY RIGHT

// Useful control registers 
#define ADXL345_REG_OFSX		0x1E
#define ADXL345_REG_OFSY		0x1F
#define ADXL345_REG_OFSZ		0x20
#define ADXL345_REG_BW_RATE		0x2C
#define ADXL345_REG_DATA_FORMAT	0x31

// Data Registers (updated based on data rate register)
#define ADXL345_REG_DATAX0		0x32
#define ADXL345_REG_DATAX1		0x33
#define ADXL345_REG_DATAY0		0x34
#define ADXL345_REG_DATAY1		0x35
#define ADXL345_REG_DATAZ0		0x36
#define ADXL345_REG_DATAZ1		0x37


typedef struct {
	guint8 i2c_addr;
	GMutex *mutex;

	// As in the registers described in the docs
	gfloat x_offset;
	gfloat y_offset;
	gfloat z_offset;
	guint8 data_rate;
	guint8 data_format;
	guint8 justify;

	// Data from the last poll of the ADXL345
	gboolean has_data;
	GTimeVal timestamp;
	gfloat x_data;
	gfloat y_data;
	gfloat z_data;
} ADXL345;


ADXL345 *adxl345_new(I2CBus *bus, guint8 i2c_addr, guint8 range, guint8 analog_gain);

