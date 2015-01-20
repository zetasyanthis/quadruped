#include <glib.h>
#include <unistd.h>

#include <sensors/i2c_bus.h>
#include <sensors/srf08.h>
#include <sensors/AD7991.h>

int
main(int argc, char **argv)
{
    g_thread_init(NULL);

    char *dev_filename = "/dev/i2c-2";
    I2CBus *bus = i2c_bus_open(dev_filename);
    SRF08 *sonar = srf08_new(bus, -1, -1, -1);
    AD7991 *adc = ad7991_new(bus, -1);

    i2c_bus_start_thread(bus);

    for (;;) {
        float v0, v1, v2, v3;
        if (ad7991_get_data(adc, &v0, &v1, &v2, &v3)) {
            g_print("ADC:\n");
            g_print("  vin0: %f\n", v0);
            g_print("  vin1: %f\n", v1);
            g_print("  vin2: %f\n", v2);
            g_print("  vin3: %f\n", v3);
        } else {
            g_print("No ADC data yet\n");
        }
        gint light;
        guint16 echoes[SRF08_NUM_ECHOES];
        if (srf08_get_data(sonar, &light, echoes)) {
            g_print("Sonic rangefinder:\n");
            g_print("  light: %u\n", light);
            for (int i = 0; i < SRF08_NUM_ECHOES; i++) {
                g_print("  echo %2d: %4u%s", i, echoes[i], ((i+1) % 3 ? "" : "\n"));
            }
            g_print("\n");
        } else {
            g_print("No sonic rangefinder data yet\n");
        }
        sleep(1);
    }

    i2c_bus_close(bus);
}
