#include <glib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include "rs485.h"
#include "rx28.h"
#include "rx28_simple.h"

/// Simple utility to set limits on range of motion for the RX28
/// servos based on observed motion.

#define for_each_motor(var) for (guint (var) = 0; (var) < 12; (var)++)

gint
main(gint argc, gchar **argv)
{
	if (rs485_open("/dev/ttyS1") < 0)
		g_error("couldn't open serial port.\n");

	// Ranges discovered for the joints
	guint16 min[3] = {1023, 1023, 1023};
	guint16 max[3] = {0,    0,    0   };

	g_print("Limits are currently set to:\n");
	for_each_motor(i) {
		rx28_set_torque_enable(i, 0);
		g_print("%2u: cw %4" G_GUINT16_FORMAT " ccw %4" G_GUINT16_FORMAT "\n",
				i,
				rx28_get_cw_angle_limit(i),
				rx28_get_ccw_angle_limit(i));
	}
	g_print("\n");

	struct pollfd pfd = {
		.fd = 0,
		.events = POLLIN,
		.revents = POLLIN
	};
	g_print("Beginning training.\n");
	g_print("Move leg 0,1,2 to its extremes and press enter when done.\n");
	printf("ID  Now  Min  Max\n");
	while (TRUE) {
		for (guint j = 0; j < 3; j++) {
			guint16 pos = rx28_get_present_position(j);
			if (pos < min[j]) min[j] = pos;
			if (pos > max[j]) max[j] = pos;
			printf("%2d %4" G_GUINT16_FORMAT "%5" G_GUINT16_FORMAT
				  "%5" G_GUINT16_FORMAT "\n", j, pos, min[j], max[j]);
		}
		if (poll(&pfd, 1, 50) <= 0) {
			printf("\x1b[12A\r"); // Back up to the top
		} else {
			tcflush(0, TCIFLUSH); // Discard stuff typed
			break;
		}
	}

	// Confirm making the changes
	gchar input = '?';
	do {	
		printf("Set limits as above? [Yn] ");
		if (scanf("%c", &input) == 0) {
			input = 'y';
		}
		if (input == 'Y' || input == '\n') {
			input = 'y';
		}
	} while (input != 'y' && input != 'n');

#define NORMAL(joint) {min[joint], max[joint]}
#define REVERSE(joint) {1023 - max[joint], 1023 - min[joint]}

	guint16 limits[12][2] = {
		NORMAL (0), // Leg 0,1,2 (front left)
		NORMAL (1),
		NORMAL (2),
		REVERSE(0), // Leg 3,4,5 (front right)
		REVERSE(1),
		REVERSE(2),
		NORMAL (0), // Leg 6,7,8 (rear right)
		NORMAL (1),
		NORMAL (2),
		REVERSE(0), // Leg 9,10,11 (rear left) 
		REVERSE(1),
		REVERSE(2)
	};

#undef NORMAL
#undef REVERSE

	if (input == 'y') {
		g_print("Setting limits:\n");
		g_print("ID  Min  Max\n");
		for_each_motor(i) {
			g_print("%2d %4d %4d\n", i, limits[i][0], limits[i][1]);
			rx28_set_cw_angle_limit(i, limits[i][0]);
			rx28_set_ccw_angle_limit(i, limits[i][1]);
		}
	}

	rs485_close();
	return 0;
}

