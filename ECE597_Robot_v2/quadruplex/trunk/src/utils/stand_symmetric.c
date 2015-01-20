#include <glib.h>
#include <stdlib.h>

#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28.h>
#include <rx28/rx28_simple.h>
#include <kinematics/inverse.h>

#define SPEED 250

gint
main(gint argc, gchar **argv)
{

	float right_front[3];
	float left_front[3];
	float left_back[3];
	float right_back[3];
	float* legs[4] = {left_front, right_front, right_back, left_back};
	struct rx28_motor_movement mm[12];
	float input[3];

	if (rs485_open("/dev/ttyS1") < 0)
		g_error("couldn't open serial port");
	
	if (argc == 1) {
		input[0] = 8.5;
		input[1] = 1.5;
		input[2] = 12;
	} else if (argc == 4) {
		input[0] = atof(argv[1]);
		input[1] = atof(argv[2]);
		input[2] = atof(argv[3]);
	} else {
        g_printerr("usage: %s [x y z]\n", argv[0]);
        exit(0);
    }
	//g_print("%s -> %f\n%s -> %f\n%s -> %f\n", argv[1], input[0], argv[2], input[1], argv[3], input[2]);
	

	kin_left_front_angles(input, legs[0]);
	kin_right_front_angles(input, legs[1]);
	kin_right_back_angles(input, legs[2]);
	kin_left_back_angles(input, legs[3]);

	g_print("Left Front:\t%+f\t%+f\t%+f\n",  legs[0][0], legs[0][1], legs[0][2]);
	g_print("Right Front:\t%+f\t%+f\t%+f\n", legs[1][0], legs[1][1], legs[1][2]);
	g_print("Right Back:\t%+f\t%+f\t%+f\n",  legs[2][0], legs[2][1], legs[2][2]);
	g_print("Left Back:\t%+f\t%+f\t%+f\n",   legs[3][0], legs[3][1], legs[3][2]);

	g_print("\n");

	for_each_leg (i) {
		for_each_joint (j) {
			legs[i][j] = rx28_rad_to_motor_val(legs[i][j]);
			guint id = MOTOR_ID(i, j);
			RX28_MOTOR_MOVEMENT(mm[id], id, legs[i][j], SPEED);
			//RX28_MOTOR_MOVEMENT(mm[3*i+j], 3*i+j, 512, 1023);
		}
	}

	g_print("Left Front:\t%f\t%f\t%f\n",  legs[0][0], legs[0][1], legs[0][2]);
	g_print("Right Front:\t%f\t%f\t%f\n", legs[1][0], legs[1][1], legs[1][2]);
	g_print("Right Back:\t%f\t%f\t%f\n",  legs[2][0], legs[2][1], legs[2][2]);
	g_print("Left Back:\t%f\t%f\t%f\n",   legs[3][0], legs[3][1], legs[3][2]);
	
	rx28_move_motors(mm, 12);
	
	return 0;
}
