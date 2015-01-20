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


/// Create and initialize a new I2CBus.
I2CBus *
i2c_bus_open(gchar *filename)
{
    I2CBus *bus = g_new0(I2CBus, 1);
    bus->dev_filename = g_strdup(filename);

	if ((bus->dev_fd = open(bus->dev_filename, O_RDWR)) < 0) {
		g_error("Unable to open %s: %s", bus->dev_filename, g_strerror(errno));
	}

    return bus;
}

void
i2c_bus_close(I2CBus *bus)
{
    close(bus->dev_fd);
}

/// Add a driver (a function that is called periodically, interleved with other drivers
/// using the I2C bus) to the bus's array.
///
/// Note: This operation is not thread-safe.  All drivers should be added before
/// i2c_bus_start_thread() is called.
gboolean
i2c_bus_add_driver(I2CBus *bus, i2c_driver_callback callback, void *user_data)
{
    if (bus->num_drivers >= I2C_DRIVERS_MAX) {
        return FALSE;
    }
    bus->drivers[bus->num_drivers] = callback;
    bus->user_data[bus->num_drivers] = user_data;
    bus->num_drivers++;
    return TRUE;
}

static gpointer
i2c_bus_thread(gpointer bus_p)
{
    I2CBus *bus = (I2CBus*) bus_p;
    while (TRUE) {
        for (int i = 0; i < bus->num_drivers; i++) {
            i2c_driver_callback callback = bus->drivers[i];
            callback(bus, bus->user_data[i]);
        }
        usleep(1000);
    }
    return NULL;
}

/// Start the I2C communication thread.  All I/O with the I2C bus is performed in this
/// thread.  Only call this after all drivers have been initialized.
void
i2c_bus_start_thread(I2CBus *bus)
{
    GError *thread_error = NULL;
    g_thread_create(i2c_bus_thread, bus, FALSE, &thread_error);
    if (thread_error != NULL) {
        g_error("Unable to start I2C thread for %s", bus->dev_filename);
    }
}

static gboolean
ioctl_i2c_mode(I2CBus *bus, int i2c_addr, gboolean report_errors)
{
    g_assert(i2c_addr >= I2C_ADDR_MIN && i2c_addr <= I2C_ADDR_MAX);

    if (ioctl(bus->dev_fd, I2C_SLAVE, i2c_addr) < 0) {
        if (report_errors) {
            g_printerr("Error with I2C ioctl() of %s, fd %d, address 0x%02x: %s\n",
                       bus->dev_filename, bus->dev_fd, i2c_addr, g_strerror(errno));
        }
        return FALSE;
    }
    return TRUE;
}


gboolean
i2c_bus_read_buffer(I2CBus *bus, int i2c_addr, int reg, gssize len, guint8 buffer[len], gboolean report_errors)
{
    g_assert(i2c_addr <= I2C_ADDR_MAX);
    g_assert(reg <= 0xFF);

    if (i2c_addr >= 0 && !ioctl_i2c_mode(bus, i2c_addr, report_errors)) {
        return FALSE;
    }

    if (reg >= 0) {
        guint8 addr_buffer[1];
        addr_buffer[0] = reg;
        if (write(bus->dev_fd, addr_buffer, 1) != 1) {
            if (report_errors) {
                g_printerr("Error with I2C write to %s, address 0x%02x: %s\n",
                           bus->dev_filename, i2c_addr, g_strerror(errno));
            }
            return FALSE;
        }
    }

    if (read(bus->dev_fd, buffer, len) != len) {
        if (report_errors) {
            g_printerr("Error with I2C read from %s, address 0x%02x: %s\n",
                       bus->dev_filename, i2c_addr, g_strerror(errno));
        }
        return FALSE;
    }
    return TRUE;
}


/// Convenience function for reading a byte from the I2C bus.
/// 
/// If `i2c_addr` is specified (non-negative) it performs the ioctl() on the bus.
/// If `reg` is specified (non-negative) it selects the register address before
/// reading.
/// 
/// On error, prints a message and returns -1.
int
i2c_bus_read_byte(I2CBus *bus, int i2c_addr, int reg, gboolean report_errors)
{
    guint8 read_buffer[1] = { 0 };
    if (i2c_bus_read_buffer(bus, i2c_addr, reg, 1, read_buffer, report_errors)) {
        return read_buffer[0];
    } else {
        return -1;
    }
}


gboolean
i2c_bus_write_buffer(I2CBus *bus, int i2c_addr, int reg, gssize len, guint8 buffer[len], gboolean report_errors)
{
    g_assert(reg >= -1);
    g_assert(i2c_addr <= I2C_ADDR_MAX);
    g_assert(reg >= -1);
    g_assert(reg <= 0xFF);

    if (i2c_addr >= 0 && !ioctl_i2c_mode(bus, i2c_addr, report_errors)) {
        return FALSE;
    }

    gssize write_len = -1;
    guint8 *write_buffer;

    // The buffer must be prefixed with the register, if a register is specified.
    guint8 prefixed_buffer[len + 1];
    if (reg != -1) {
        prefixed_buffer[0] = reg;
        memcpy(prefixed_buffer + 1, buffer, len);
        write_buffer = prefixed_buffer;
        write_len = len + 1;
    } else {
        write_buffer = buffer;
        write_len = len;
    }

    if (write(bus->dev_fd, write_buffer, write_len) != write_len) {
        if (report_errors) {
            g_printerr("Error with I2C write of %" G_GSSIZE_FORMAT " bytes to %s, address 0x%02x: %s\n",
                       write_len, bus->dev_filename, i2c_addr, g_strerror(errno));
        }
        return FALSE;
    }

    return TRUE;
}


gboolean
i2c_bus_write_byte(I2CBus *bus, int i2c_addr, int reg, guint8 byte, gboolean report_errors)
{
    guint8 buffer[1] = { byte };
    return i2c_bus_write_buffer(bus, i2c_addr, reg, 1, buffer, report_errors);
}
