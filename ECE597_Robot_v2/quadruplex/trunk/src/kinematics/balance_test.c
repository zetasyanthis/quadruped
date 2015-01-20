#include <glib.h>
#include <stdlib.h>
#include "balance.h"

int main(gint argc, gchar **argv)
{
	float foot_positions[4][3] = {
		{ -1,  1,  0},
		{  1,  1,  0},
		{  1, -1,  0},
		{ -1, -1,  0}
	};
	
	float cog[4][3] = {
		{  0, 10,  0},
		{ 10,  0,  0},
		{  0,-10,  0},
		{-10,  0,  0}
	};

	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			float stable = kin_check_stability_cross(foot_positions, cog[i], j);
			g_print("Leg %d Moving:\t%f\n", j, stable);
		}
	}
	
	return 0;
}

