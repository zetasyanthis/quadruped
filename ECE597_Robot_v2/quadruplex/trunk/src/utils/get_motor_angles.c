#define _GNU_SOURCE

#include <unistd.h>
#include <glib.h>

#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28.h>
#include <rx28/rx28_simple.h>

gint
main(gint argc, gchar **argv)
{
//	for (guint16 i = 0; i < 1024; i++) {
//		g_print("%4d: %.4f round-trip %4d\n",
//				i,
//				motor_val_to_rad(i),
//				rad_to_motor_val(motor_val_to_rad(i)));
//	}

	if (rs485_open("/dev/ttyS1") < 0)
		g_error("couldn't open serial port");
	
	guint16 present_positions[12] = {0};

	while (TRUE) {
		// Get the present position of each motor
		for_each_motor(i) {
			present_positions[i] = rx28_get_present_position(i);
		}

		for_each_leg(i) {
			float theta1 = rx28_motor_val_to_rad(present_positions[i + 0]);
			float theta2 = rx28_motor_val_to_rad(present_positions[i + 1]);
			float theta3 = rx28_motor_val_to_rad(present_positions[i + 2]);
			
			g_print("%d %f %f %f\n", i, theta1, theta2, theta3);
		}
		usleep(1000);
	}

	return 0;
}

