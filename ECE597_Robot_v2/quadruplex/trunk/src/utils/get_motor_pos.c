#define _GNU_SOURCE

#include <unistd.h>
#include <glib.h>

#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28.h>
#include <rx28/rx28_simple.h>
#include <kinematics/forward.h>
#include <kinematics/kin_helper.h>

gint
main(gint argc, gchar **argv)
{
	gboolean do_loop = argc > 1;

//	for (guint16 i = 0; i < 1024; i++) {
//		g_print("%4d: %.4f round-trip %4d\n",
//				i,
//				motor_val_to_rad(i),
//				rad_to_motor_val(motor_val_to_rad(i)));
//	}

	if (rs485_open("/dev/ttyS1") < 0)
		g_error("couldn't open serial port");
	
	guint16 present_positions[12] = {0};
	// Output variables for kin_forward
	float motor1[3], motor2[3], foot[3];
	float motor1_cog[3], motor2_cog[3], foot_cog[3];

	g_print("Leg    x        y       z\n");
	do {
		// Get the present position of each motor
		for_each_motor(i) {
			present_positions[i] = rx28_get_present_position(i);
		}

		for_each_leg(i) {
			float theta1 = rx28_motor_val_to_rad(present_positions[i * 3 + 0]);
			float theta2 = rx28_motor_val_to_rad(present_positions[i * 3 + 1]);
			float theta3 = rx28_motor_val_to_rad(present_positions[i * 3 + 2]);
			//g_print("Thetas: %2.4f %2.4f %2.4f\n", theta1, theta2, theta3);
			if (i == 0 || i == 2) {
				theta1 = -theta1;
				theta2 = -theta2;
				theta3 = -theta3;
			}
			//g_print("Thetas: %2.4f %2.4f %2.4f\n", theta1, theta2, theta3);
			kin_forward(motor1, motor2, foot,
					motor1_cog, motor2_cog, foot_cog,
					theta1, theta2, theta3);
			g_print("%2d %+8.4f %+8.4f %+8.4f\n", i, motor1[0], motor1[1], motor1[2]);
			g_print("%2d %+8.4f %+8.4f %+8.4f\n", i, motor2[0], motor2[1], motor2[2]);
			g_print("%2d %+8.4f %+8.4f %+8.4f\n", i, foot[0], foot[1], foot[2]);
			g_print("\n");
		}
		if (do_loop) {
			usleep(1000);
			g_print("\x1b[%dA\r", 16); // Go up lines
		}
	} while (do_loop);

	return 0;
}


//	Y is referenced to be forward for the robot
//	X is referenced to be to the left of the robot
//	Z is referenced to be down from the robot shoulder

