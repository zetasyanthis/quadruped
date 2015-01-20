#include <glib.h>

#define AD7991_ADDRESS 0b00101001

typedef struct {
    guint8 i2c_addr;
    GMutex *mutex;

    // Data, protected by the mutex
    gboolean has_data;
    float values[4];
} AD7991;


AD7991 *ad7991_new(I2CBus *bus, int i2c_addr);
gboolean ad7991_get_data(AD7991 *adc, float *value0, float *value1, float *value2, float *value3);

