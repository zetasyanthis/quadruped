#include <glib.h>
#include <stdlib.h>
#include "forward.h"

int main(gint argc, gchar **argv)
{
	float motor1[3]	= {0};
	float motor2[3]	= {0};
	float foot[3]	= {0};
	
	float motor1_cog[3]	= {0};
	float motor2_cog[3]	= {0};
	float foot_cog[3]	= {0};
	
	//float test[3][3];
	
	if (argc != 4) {
		g_printerr("Need 3 float arguments\n");
		return 2;
	}
	
	float input[3] = {atof(argv[1]), atof(argv[2]), atof(argv[3])};
	
	kin_forward(motor1, motor2, foot, motor1_cog, motor2_cog, foot_cog, input[0], input[1], input[2]);

	g_print("\n");
	g_print("%f %f %f\n", motor1[0], motor1[1], motor1[2]);
	g_print("%f %f %f\n", motor1_cog[0], motor1_cog[1], motor1_cog[2]);
	g_print("\n");
	g_print("%f %f %f\n", motor2[0], motor2[1], motor2[2]);
	g_print("%f %f %f\n", motor2_cog[0], motor2_cog[1], motor2_cog[2]);
	g_print("\n");
	g_print("%f %f %f\n", foot[0], foot[1], foot[2]);
	g_print("%f %f %f\n", foot_cog[0], foot_cog[1], foot_cog[2]);
	
//	g_print("%f %f %f\n", s1[0]+s2[0]+s3[0], s1[1]+s2[1]+s3[1], s1[2]+s2[2]+s3[2]);			This is the output with 0 0 0 input.
	
	return 0;
}

