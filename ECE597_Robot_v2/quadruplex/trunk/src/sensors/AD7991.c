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

#include <sensors/i2c_bus.h>
#include <sensors/AD7991.h>


static void
i2c_driver(I2CBus *bus, gpointer ad7991_p)
{
    AD7991 *adc = (AD7991*) ad7991_p;

	guint8 buffer[8] = {0};
    if (!i2c_bus_read_buffer(bus, adc->i2c_addr, -1, 8, buffer, TRUE)) {
        return;
    }

    //g_print("Read from the ADC\n");
	
    g_mutex_lock(adc->mutex);
    adc->has_data = TRUE;
    for (int i = 0; i < 4; i++) {
        guint8 high = buffer[i * 2];
        guint8 low = buffer[i * 2 + 1];
        float value = (((high & 0b00001111) << 8) + low) / 4096.0 * 5.0;
		gssize channel = (high & 0b00110000) >> 4;
        adc->values[channel] = value;
        //g_print("Channel %d: %f\n", channel, value);
	}
    g_mutex_unlock(adc->mutex);
}


AD7991 *
ad7991_new(I2CBus *bus, int i2c_addr)
{
    g_assert(i2c_addr == -1 || (i2c_addr >= I2C_ADDR_MIN && i2c_addr <= I2C_ADDR_MAX));
    if (i2c_addr == -1) {
        i2c_addr = AD7991_ADDRESS;
    }

    AD7991 *adc = g_new0(AD7991, 1);

    adc->i2c_addr = i2c_addr;
    adc->mutex = g_mutex_new();

    i2c_bus_add_driver(bus, i2c_driver, adc);
    return adc;
}


gboolean
ad7991_get_data(AD7991 *adc, float *value0, float *value1, float *value2, float *value3)
{
    gboolean ret = FALSE;
    g_mutex_lock(adc->mutex);
    if (adc->has_data) {
        if (value0 != NULL) *value0 = adc->values[0];
        if (value1 != NULL) *value1 = adc->values[1];
        if (value2 != NULL) *value2 = adc->values[2];
        if (value3 != NULL) *value3 = adc->values[3];
        ret = TRUE;
    }
    g_mutex_unlock(adc->mutex);
    return ret;
}
