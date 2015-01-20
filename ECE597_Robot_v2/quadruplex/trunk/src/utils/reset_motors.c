#define _GNU_SOURCE

#include <unistd.h>
#include <glib.h>

#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28_simple.h>

gint
main(gint argc, gchar **argv)
{
	if (rs485_open("/dev/ttyS1") < 0)
		g_error("Unable to open serial port");

	for_each_motor(id) {
		rx28_set_torque_limit(id, 0x03ff);
	}
}

