#include <glib.h>
#include <stdlib.h>
#include <math.h>

#include <helper.h>
#include <kinematics/leg_defs.h>
#include <kinematics/nn_defs.h>
#include <kinematics/inverse.h>


/** Calculate the output of a sigmoid neural network node with a bias term
*	
*	\param dim		Number of inputs to the node
*	\param inputs	Inputs from previous layer
*	\param weights	Weights for each input
*	\param bias		Bias term added to the sum before the sigmoid is calculated
*/
static inline float sigmoid_with_bias(int dim, const float inputs[dim], const float weights[dim], float bias)
{
	float output = 0.0;
	for (int i = 0; i < dim; i++) {
		output += inputs[i] * weights[i];
	}
	output += bias;
	return 1.0 / (1.0 + exp(-output));
}

/** Calculate the output of a linear neural network node with a bias term
*	
*	\param dim		Number of inputs to the node
*	\param inputs	Inputs from previous layer
*	\param weights	Weights for each input
*	\param bias		Bias term added to the sum
*/
static inline float linear_with_bias(int dim, const float inputs[dim], const float weights[dim], float bias)
{
	float output = 0.0;
	for (int i = 0; i < dim; i++) {
		output += inputs[i] * weights[i];
	}
	return output + bias;
}

/** Calculate the output of the 3-10-10-3 neural network
*	\param input	The x,y,z foot position
*	\param output	The motor angles to achieve that position
*/
void nn_activate(float input[3], float output[3])
{
	float hidden0[10];
	float hidden1[10];
	for (int i = 0; i < 10; i++) {
		hidden0[i] = sigmoid_with_bias(3, input, input_to_hidden0[i], bias_to_hidden0[i]);
	}
	for (int i = 0; i < 10; i++) {
		hidden1[i] = sigmoid_with_bias(10, hidden0, hidden0_to_hidden1[i], bias_to_hidden1[i]);
	}
	for (int i = 0; i < 3; i++) {
		output[i] = linear_with_bias(10, hidden1, hidden1_to_output[i], bias_to_output[i]);
	}
}

void kin_robot_coords_to_leg_coords(float leg[3], float robot[3], guint leg_number)
{
	switch (leg_number) {
		case 0:		// Left front
			leg[0] = (-robot[0]) - leg_offsets[0];
			leg[1] = ( robot[1]) - leg_offsets[1];
			leg[2] = ( robot[2]) - leg_offsets[2];
			break;
		case 1:		// Right front
			leg[0] = ( robot[0]) - leg_offsets[0];
			leg[1] = ( robot[1]) - leg_offsets[1];
			leg[2] = ( robot[2]) - leg_offsets[2];
			break;
		case 2:		// Right back
			leg[0] = ( robot[0]) - leg_offsets[0];
			leg[1] = (-robot[1]) - leg_offsets[1];
			leg[2] = ( robot[2]) - leg_offsets[2];
			break;
		case 3:		// Left back
			leg[0] = (-robot[0]) - leg_offsets[0];
			leg[1] = (-robot[1]) - leg_offsets[1];
			leg[2] = ( robot[2]) - leg_offsets[2];
			break;
	}
}

void kin_leg_coords_to_robot_coords(float robot[3], float leg[3], guint leg_number)
{
	switch (leg_number) {
		case 0:
			robot[0] = -(leg[0] + leg_offsets[0]);
			robot[1] =  (leg[1] + leg_offsets[1]);
			robot[2] =  (leg[2] + leg_offsets[2]);
			break;
		case 1:
			robot[0] =  (leg[0] + leg_offsets[0]);
			robot[1] =  (leg[1] + leg_offsets[1]);
			robot[2] =  (leg[2] + leg_offsets[2]);
			break;
		case 2:
			robot[0] =  (leg[0] + leg_offsets[0]);
			robot[1] = -(leg[1] + leg_offsets[1]);
			robot[2] =  (leg[2] + leg_offsets[2]);
			break;
		case 3:
			robot[0] = -(leg[0] + leg_offsets[0]);
			robot[1] = -(leg[1] + leg_offsets[1]);
			robot[2] =  (leg[2] + leg_offsets[2]);
			break;
	}
}

void kin_leg_coords_to_motor_angles(float angles[3], float leg_coords[3], guint leg_number)
{
	switch (leg_number) {
		case 0:
			kin_left_front_angles(leg_coords, angles);
			break;
		case 1:
			kin_right_front_angles(leg_coords, angles);
			break;
		case 2:
			kin_right_back_angles(leg_coords, angles);
			break;
		case 3:
			kin_left_back_angles(leg_coords, angles);
			break;
	}
}

inline void kin_left_front_angles(float xyz_in[], float theta_out[])
{
	nn_activate(xyz_in, theta_out);
	theta_out[0] = -theta_out[0];
	theta_out[1] = -theta_out[1];
	theta_out[2] = -theta_out[2];
}

inline void kin_right_front_angles(float xyz_in[], float theta_out[])
{
	nn_activate(xyz_in, theta_out);
}

inline void kin_right_back_angles(float xyz_in[], float theta_out[])
{
	nn_activate(xyz_in, theta_out);
	theta_out[0] = -theta_out[0];
	theta_out[1] = -theta_out[1];
	theta_out[2] = -theta_out[2];
}

inline void kin_left_back_angles(float xyz_in[], float theta_out[])
{
	nn_activate(xyz_in, theta_out);
}

