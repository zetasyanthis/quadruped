#include <glib.h>
#include <stdlib.h>
#include "inverse.h"

#define for_each_leg(i) for (guint (i) = 0; (i) < 4; (i)++)

int main(gint argc, gchar **argv) {
	float output[3] = {0};
	if (argc != 4) {
		g_printerr("Need 3 float arguments\n");
		return 2;
	}
	float input[3] = {atof(argv[1]), atof(argv[2]), atof(argv[3])};
	//g_print("%s -> %f\n%s -> %f\n%s -> %f\n", argv[1], input[0], argv[2], input[1], argv[3], input[2]);
	//nn_activate(input, output);
	for_each_leg(i) {
		kin_robot_coords_to_leg_coords(output, input, i);
		g_print("%d %f %f %f\n", i, output[0], output[1], output[2]);
	}
	return 0;
}

