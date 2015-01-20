#include <glib.h>
#include <unistd.h>
#include <string.h>

#include <sensors/i2c_bus.h>
#include <sensors/srf08.h>

/// Helper function to send the ranging command.  Returns TRUE if it succeeds, else FALSE.
static gboolean
send_ranging_command(SRF08 *sonar, I2CBus *bus)
{
    if (!i2c_bus_write_byte(bus, sonar->i2c_addr, SRF08_COMMAND_REG, SRF08_COMMAND_RANGE_CM, TRUE)) {
        return FALSE;
    }
    return TRUE;
}


// Helper function to check whether the SRF08 has stopped ranging.  The data
// sheet says that all reads will return 255 while it is ranging (because it
// ignores I2C entirely), but this shows up in Linux as an I/O error, so errors
// are simply taken as an indication that it is ranging.
//
// This could potentially be improved by inspecting errno for a specific error,
// but that hardly seems necessary.
static gboolean
is_ranging_complete(SRF08 *sonar, I2CBus *bus)
{
    int byte = i2c_bus_read_byte(bus, sonar->i2c_addr, 0x00, FALSE);
    return byte >= 0 && byte != 0xFF;
}

/// Helper function to read data from the SRF08 and store it in the struct.
static gboolean
update_data(SRF08 *sonar, I2CBus *bus)
{
    g_get_current_time(&sonar->timestamp);

    int light = i2c_bus_read_byte(bus, -1, 0x01, TRUE);
    if (light >= 0) {
        sonar->light = light;
    } else {
        return FALSE;
    }

    guint8 echo_bytes[SRF08_NUM_ECHOES * 2];
    if (!i2c_bus_read_buffer(bus, -1, SRF08_ECHO_REG, SRF08_NUM_ECHOES * 2, echo_bytes, TRUE)) {
        return FALSE;
    }

    for (int i = 0; i < SRF08_NUM_ECHOES; i++) {
        int high = echo_bytes[i * 2];
        int low  = echo_bytes[i * 2 + 1];
        sonar->echoes[i] = high << 8 | low;
    }

    return TRUE;
}

/// Endlessly try to update the rangefinder data.
static void
i2c_driver(I2CBus *bus, gpointer sonar_p)
{
    SRF08 *sonar = (SRF08*) sonar_p;

    switch (sonar->state) {
        case SRF08_STATE_INIT:
            // This SRF08 instance is newly created; set the range and gain
            {
                guint8 buffer[2] = { sonar->analog_gain, sonar->range };
                if (i2c_bus_write_buffer(bus, -1, SRF08_GAIN_REG, 2, buffer, TRUE)) {
                    sonar->state = SRF08_STATE_START_RANGING;
                }
            }
            break;
        case SRF08_STATE_START_RANGING:
            // Send the command to start ranging
            if (send_ranging_command(sonar, bus)) {
                sonar->state = SRF08_STATE_WAITING;
            }
            break;
        case SRF08_STATE_WAITING:
            // Check if the ranging has completed and load the data if it has
            if (is_ranging_complete(sonar, bus)) {
                g_mutex_lock(sonar->mutex);
                if (update_data(sonar, bus)) {
                    sonar->has_data = TRUE;
                    sonar->state = SRF08_STATE_START_RANGING;
                }
                g_mutex_unlock(sonar->mutex);
            }
            break;
    }
}


SRF08 *
srf08_new(I2CBus *bus, int i2c_addr, int analog_gain, int range)
{
    g_assert(i2c_addr == -1 || (i2c_addr >= I2C_ADDR_MIN && i2c_addr <= I2C_ADDR_MAX));
    g_assert(analog_gain == -1 || (analog_gain >= SRF08_ANALOG_GAIN_MIN && analog_gain <= SRF08_ANALOG_GAIN_MAX));
    g_assert(range == -1 || (range >= SRF08_RANGE_MIN && range <= SRF08_RANGE_MAX));

    if (i2c_addr == -1)    i2c_addr = SRF08_DEFAULT_ADDRESS;
    if (analog_gain == -1) analog_gain = SRF08_DEFAULT_ANALOG_GAIN;
    if (range == -1)       range = SRF08_DEFAULT_RANGE;

    SRF08 *sonar = g_new0(SRF08, 1);

    sonar->i2c_addr = i2c_addr;
    sonar->state = SRF08_STATE_INIT;
    sonar->mutex = g_mutex_new();

    sonar->range = range;
    sonar->analog_gain = analog_gain;

    i2c_bus_add_driver(bus, i2c_driver, sonar);
    return sonar;
}


/// Copy data from the SRF08 struct to a local buffer, doing the appropriate locking
/// to assure a consistent appearance.  Return FALSE if no data is available (leaving
/// the arguments untouched), else TRUE.
gboolean
srf08_get_data(SRF08 *sonar, gint *light, guint16 echoes[SRF08_NUM_ECHOES])
{
    gboolean ret = TRUE;
    g_mutex_lock(sonar->mutex);
    if (sonar->has_data) {
        if (light != NULL) {
            *light = sonar->light;
        }
        if (echoes != NULL) {
            memcpy(echoes, sonar->echoes, sizeof(guint16) * SRF08_NUM_ECHOES);
        }
    } else {
        ret = FALSE;
    }
    g_mutex_unlock(sonar->mutex);
    return ret;
}

