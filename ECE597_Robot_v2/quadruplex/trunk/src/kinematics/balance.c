#include <glib.h>
#include "balance.h"
#include "forward.h"
#include "inverse.h"
#include "../helper.h"
#include "kin_helper.h"

static float cog_main_body_pos[3] = {0,0,0};

/** Calculates the center of mass in robot coordinates.  
* 
* This function takes a weighted average of various parts of the
* robot modeled as point masses to calculate the center of gravity.  
* 
* \note This function currently returns 0 for the z-component of the cog.
* 
* \todo move cog_main_body_pos into leg_defs.py or 
* 
* \param leg_angles		The leg motor angles in radians.
* \param cog			Filled by function.  The xyz position of the cog in robot coordinates.
*/
void
kin_center_of_gravity(float cog[3], float leg_angles[4][3])
{
	float sum[3] = {0};
	float motor_cogs[12][3];

	for_each_leg (leg) {
		float theta1 = leg_angles[leg][0];
		float theta2 = leg_angles[leg][1];
		float theta3 = leg_angles[leg][2];
		if (leg == 0 || leg == 2) {
			theta1 = -theta1;
			theta2 = -theta2;
			theta3 = -theta3;
		}

		kin_forward_cog(motor_cogs[MOTOR_ID(leg, 0)],
						motor_cogs[MOTOR_ID(leg, 1)],
						motor_cogs[MOTOR_ID(leg, 2)],
						theta1, theta2, theta3);
		kin_leg_coords_to_robot_coords(motor_cogs[MOTOR_ID(leg, 0)],
										motor_cogs[MOTOR_ID(leg, 0)],
										leg);
		kin_leg_coords_to_robot_coords(motor_cogs[MOTOR_ID(leg, 1)],
										motor_cogs[MOTOR_ID(leg, 1)],
										leg);
		kin_leg_coords_to_robot_coords(motor_cogs[MOTOR_ID(leg, 2)],
										motor_cogs[MOTOR_ID(leg, 2)],
										leg);
	}
	
	for_each_motor (j) {
		for_each_axis (i) {
			if (i % 3 == 2) {
				sum[i] += motor_cogs[j][i] * MASS_OF_FOOT;
			} else {
				sum[i] += motor_cogs[j][i] * MASS_OF_MOTOR;
			}
		}
	}
	
	for_each_axis(i) {
		sum[i] += cog_main_body_pos[i] * MASS_OF_MAIN_BODY;
		cog[i] = sum[i]/(13.0 * MASS_OF_ROBOT);
	}
	cog[2] = 0;
}

/** Calculates the center of the stability triangle
* 
* This is calculated via the centroid of a triangle formed by the foot
* positions of the three non-moving legs.
* 
* \note This function currently returns 0 for the z-component of the centroid.
* 
* \param vertices		The foot positions in robot coordinates.
* \param foot_to_skip	This is utilized to skip the appropriate leg.  This seems a bit hack-ish, but it necessary to simplify the calling function.
* \param centroid		Filled by function.  The xyz position of the centroid in robot coordinates.
* 
*/
void
kin_triangle_centroid(float centroid[3], guint foot_to_skip, float vertices[4][3])
{
	// This skips the z calculation since we're using foot positions and z will be at that level.

	for (int i = 0; i < 2; i++) {	
		centroid[i] = 0;
		for_each_leg (leg) {
			if (leg != foot_to_skip) {
				centroid[i] += vertices[leg][i];
			}
		}
		centroid[i] /= 3.0;
	}
	centroid[2] = 0;
}

/** Checks to see if we've crossed into the stability triangle
* 
* \param foot_positions		The foot positions in robot coordinates.
* \param cog				The center of gravity of the robot.
* \param moving_leg			The leg that is moving.
* 
* \return 					Positive value if inside stability triangle, negative if outside, zero if on the line exactly.
*/
float
kin_check_stability_cross(float foot_positions[4][3], float cog[3], int moving_leg)
{
	unsigned char leg0 = 0;
	unsigned char leg1 = 0;
	
	switch(moving_leg) {
		case 0:
			leg0 = 1;
			leg1 = 3;
			break;
		case 1:
			leg0 = 2;
			leg1 = 0;
			break;
		case 2:
			leg0 = 3;
			leg1 = 1;
			break;
		case 3:
			leg0 = 0;
			leg1 = 2;
			break;
	}
	
	return ((foot_positions[leg1][0] - foot_positions[leg0][0]) * (cog[1] - foot_positions[leg0][1]) - (foot_positions[leg1][1] - foot_positions[leg0][1]) * (cog[0] - foot_positions[leg0][0]));
}

