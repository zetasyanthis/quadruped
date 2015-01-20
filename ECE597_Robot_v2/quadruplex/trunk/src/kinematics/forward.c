#define _GNU_SOURCE

#include <glib.h>
#include <stdlib.h>
#include "leg_defs.h"
#include "forward.h"
#include "kin_helper.h"

/** Calculates the foot position relative to the shoulder joint from the motor angles.
* 
* This function calculates the positions of the knee and foot joints as well
* as the centers of gravity for the lower two motors motors and the foot piece.  
*
* 	\note Seperate variables must be used for all outputs, as several are used internally later on in the calculation.
*	
*	\todo Need to adjust tip of foot by using a vector to the middle of the foot and adding the radius to the z term.
*	
*	\param theta1	Angle of shoulder joint.	(0 value referenced to knee motor at 90 deg to shoulder motor.)
*	\param theta2	Angle of knee joint.		(0 value referenced to foot motor at 0 deg to knee motor.)
*	\param theta3	Angle of foot joint.		(0 value referenced to foot at 90 deg to foot motor.)
*
*	\param motor1		Filled by function.  The xyz position of the knee motor pivot.
*	\param motor2		Filled by function.  The xyz position of the foot motor pivot.
*	\param foot			Filled by function.  The xyz position of the foot tip.
*	\param motor1_cog	Filled by function.  The xyz position of the cog of the knee motor.
*	\param motor2_cog	Filled by function.  The xyz position of the cog of the foot motor.
*	\param foot_cog		Filled by function.  The xyz position of the cog of the foot.	
*/
void kin_forward(float motor1[3], float motor2[3], float foot[3],
		float motor1_cog[3], float motor2_cog[3], float foot_cog[3],
		float theta1, float theta2, float theta3)
{
	float rot1[3][3];
	float rot2[3][3];
	float rot3[3][3];

	float mat2[3][3];
	float mat3[3][3];
	
	// Fill rotation matrices
	kin_fill_ry(rot1, -theta1);
	kin_fill_rx(rot2, theta2);
	kin_fill_rx(rot3, theta3);
	
	// Calculate first joint location and cog for motor 0/3/6/9.
	kin_mat_vect_product(motor1, rot1, s1);
	kin_mat_vect_product(motor1_cog, rot1, s1_cog);
	
/*	g_print("\n");
	g_print("%f %f %f\n", motor1[0], motor1[1], motor1[2]);
	g_print("%f %f %f\n", motor1_cog[0], motor1_cog[1], motor1_cog[2]);*/
	
	// Calculate next set of rotation matrices in advance
	kin_mat_product(mat2, rot1, rot2);
	kin_mat_product(mat3, mat2, rot3);

/*	g_print("\n");
	kin_print_mat(3,3,rot1);
	g_print("\n");
	kin_print_mat(3,3,rot2);
	g_print("\n");
	kin_print_mat(3,3,rot3);	
	g_print("\n");
	kin_print_mat(3,3,mat2);
	g_print("\n");
	kin_print_mat(3,3,mat3);*/

	// Calculate second joint location
	kin_mat_vect_product(motor2, mat2, s2);
	ADD_VECTORS(motor2, motor2, motor1);
	
	// Calculate cog for motor 1/4/7/10 (knee).
	kin_mat_vect_product(motor2_cog, mat2, s2_cog);
	ADD_VECTORS(motor2_cog, motor2_cog, motor1);
	
/*	g_print("\n");
	g_print("%f %f %f\n", motor2[0], motor2[1], motor2[2]);
	g_print("%f %f %f\n", motor2_cog[0], motor2_cog[1], motor2_cog[2]);*/
	
	// Calculate third joint location
	kin_mat_vect_product(foot, mat3, s3);	
	ADD_VECTORS(foot, foot, motor2);
	
	// Calculate cog for motor 2/5/8/11 (foot).
	kin_mat_vect_product(foot_cog, mat2, s2_cog);
	ADD_VECTORS(foot_cog, foot_cog, motor2);
	
/*	g_print("\n");
	g_print("%f %f %f\n", foot[0], foot[1], foot[2]);
	g_print("%f %f %f\n", foot_cog[0], foot_cog[1], foot_cog[2]);*/
}

/** Wrapper for kin_forward when only cogs are needed.
* 
* Calls kin_forward and stores the non-cog results in seperate internal variables.
* 
* 	\note Seperate variables must be used for all outputs, as several are used internally later on in the calculation.
* 
*	\param theta1	Angle of shoulder joint.	(0 value referenced to knee motor at 90 deg to shoulder motor.)
*	\param theta2	Angle of knee joint.		(0 value referenced to foot motor at 0 deg to knee motor.)
*	\param theta3	Angle of foot joint.		(0 value referenced to foot at 90 deg to foot motor.)
*
*	\param motor1_cog	Filled by function.  The xyz position of the cog of the knee motor.
*	\param motor2_cog	Filled by function.  The xyz position of the cog of the foot motor.
*	\param foot_cog		Filled by function.  The xyz position of the cog of the foot.	
*/
void kin_forward_cog(float motor1_cog[3], float motor2_cog[3], float foot_cog[3],
		float theta1, float theta2, float theta3)
{
	float motor1[3], motor2[3], foot[3];
	kin_forward(motor1, motor2, foot, motor1_cog, motor2_cog, foot_cog, theta1, theta2, theta3);
}
