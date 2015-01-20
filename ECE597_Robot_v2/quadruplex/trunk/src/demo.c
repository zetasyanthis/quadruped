#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cwiid.h>

#include <helper.h>
#include <rx28/rs485.h>
#include <rx28/rx28.h>
#include <rx28/rx28_simple.h>
#include <kinematics/inverse.h>
#include <kinematics/forward.h>
#include <kinematics/balance.h>
#include <kinematics/kin_helper.h>
#include <control/wiimote.h>

/// Unpack the three elements of foo, comma-separated.  Useful for passing to g_print()
#define ITEMS_OF(foo) foo[0], foo[1], foo[2]
/// Format for a tuple of three floating-point numbers.
#define TRIPLE_FORMAT "%+8.4f %+8.4f %+8.4f"

#define NUM_TIMESTEPS 20
#define STEP_HEIGHT   4
#define MOTOR_SPEED   200
#define SHIFT_AMOUNT  0.7
#define MAX_ANGLE (G_PI_2 / 3.0)
#define MAX_DISTANCE 5.0


/// Set the feet to go to the xyz positions (in robot coordinates) specified in
/// feet_start.  Speed is the raw motor speed to perform the action.
void 
do_movement(float feet[4][3], guint16 speed, gboolean enable_movement)
{
	struct rx28_motor_movement mm[12];
	float leg_coords[3];
	float leg_angles[3];

	for_each_leg (leg) {
		kin_robot_coords_to_leg_coords(leg_coords, feet[leg], leg);
		kin_leg_coords_to_motor_angles(leg_angles, leg_coords, leg);
		for_each_joint (j) {
			guint id = MOTOR_ID(leg, j);
			RX28_MOTOR_MOVEMENT(mm[id], id, rx28_rad_to_motor_val(leg_angles[j]), speed);
		}
	}

	if (enable_movement) {
		rx28_move_motors(mm, 12);
		sleep(1);
	}
}

/// Apply 2D rotation and translation to the array of xyz locations in current_pos.
void
fill_foot_deltas(float foot_deltas[4][3], float current_pos[4][3], float distance, float angle)
{
	float translated_pos[3];
	float rotated_pos[4][3];
	float rotation[3][3];
	float rot_angle = angle / MAX_ANGLE;
	kin_fill_rz(rotation, rot_angle);
	
	float translation[3] = {0, distance * MAX_DISTANCE * sinf(angle), 0};
	for_each_leg (leg) {
		ADD_VECTORS(translated_pos, current_pos[leg], translation);
		kin_mat_vect_product(rotated_pos[leg], rotation, translated_pos);
	}

	for_each_leg (leg) {
		SUBTRACT_VECTORS(foot_deltas[leg], rotated_pos[leg], current_pos[leg]);
	}
}

int
choose_leg(int step_index, float angle)
{
	return step_index;
	if (angle > 0) {
		switch (step_index) {
			case 0:
				return 0;
			case 1:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
		}
	} else {
		switch (step_index) {
			case 0:
				return 1;
			case 1:
				return 3;
			case 2:
				return 2;
			case 3:
				return 0;
		}
	}
	g_warn_if_reached();
	return -1;
}


gint
main(gint argc, gchar **argv)
{
	g_thread_init(NULL);
	g_type_init();
	wiimote_init();

	gboolean verbose = FALSE;//TRUE;
	gboolean enable_movement = argc > 1;

	if (enable_movement) {
		if (rs485_open("/dev/ttyS1") < 0) {
			g_error("couldn't open serial port");
		}
	}

	float foot_start_coords[3] = {8.5, 1.5, 15}; // Starting position of each leg
	float feet_start [4][3]; // Robot coordinates of where the feet are now
	float foot_deltas[4][3]; // Robot coordinates of where the feet should go, relative to starting position

	// Convert the starting leg coordinates to the robot's coordinate frame
	if (verbose) g_print("Initial foot positions\n");
	for_each_leg (i) {
		kin_leg_coords_to_robot_coords(feet_start[i], foot_start_coords, i);
		if (verbose) g_print("%d starting at %+9.4f %+9.4f %+9.4f\n", i, ITEMS_OF(feet_start[i]));
	}

	void do_stand()
	{
		if (verbose) g_print("\nAssuming standing position\n");
		do_movement(feet_start, 100, enable_movement);
	}

	do_stand();

	// Status variables
	float feet_current[4][3]; // Current XYZ positions of the feet in robot coordinates
	for_each_leg_joint (leg, joint, id) {
		feet_current[leg][joint] = feet_start[leg][joint];
	}
	float foot_leg_coords[4][3]; // XYZ foot positions in leg coordinates
	float leg_angles[4][3]; // Angles of leg joints in radians
	guint16 last_goal_pos[12]; // Previous angles of the leg joints in motor units
	for_each_motor(id) {
		last_goal_pos[id] = rx28_get_present_position(id);
	}
	float current_cog[3] = {0.0, 0.0, 0.0}; // We assume that it starts at the origin
	float centroid[3] = {0.0, 0.0, 0.0};
	float shift_delta[3] = {0.0, 0.0, 0.0};
	float cumulative_shift_delta[3] = {0}; // Cumulative shifts to move CoG
	struct rx28_motor_movement mm[12];

	// Wiimote status variables
	enum wiimote_conn_status connected = WIIMOTE_CONN_NONE;
	struct cwiid_nunchuk_mesg nunchuk = {0};

	while (TRUE) {
		wiimote_get_data(&connected, NULL, &nunchuk);
		if (connected != WIIMOTE_CONN_WIIMOTE) {
			g_print("Not connected to the wiimote\n");
			usleep(10000);
			continue;
		}
		
		float x = CLAMP((nunchuk.stick[0] - 122) / 100.0, -1.0, 1.0);
		float y = CLAMP((nunchuk.stick[1] - 133) / 100.0, -1.0, 1.0);
		if (x < 0.1 && x > -0.1) x = 0;
		if (y < 0.1 && y > -0.1) y = 0;

		float angle = atan2f(x, y);
		float distance = sqrtf(x*x + y*y);
		distance = CLAMP(distance, 0, 1.0);
		if (angle > G_PI_2 || angle < -G_PI_2) distance = -distance;
		g_print("Wiimote directions: %+1.2f rad %+1.2f distance\n", angle, distance * MAX_DISTANCE);

		if (distance < 0.1) {
			g_print("Distance near zero or negative: standing\n");
			do_stand();
			usleep(10000);
			continue;
		}

		// Generate a delta for each coordinatestepping to
		fill_foot_deltas(foot_deltas, feet_start, distance, angle);
		if (verbose) {
			g_print("\nFoot deltas:\n");
			for_each_leg (i) {
				g_print("%d delta is %+9.4f %+9.4f %+9.4f\n", i, ITEMS_OF(foot_deltas[i]));
			}
		}

		for (guint i = 0; i < 4; i++) {
			float step_delta[3];
			float feet_step_start[4][3]; // XYZ positions of the feet at the start of the step
			for_each_leg_joint (leg, joint, id) {
				feet_step_start[leg][joint] = feet_current[leg][joint];
			}
			guint stepping_leg = choose_leg(i, angle);
			kin_triangle_centroid(centroid, stepping_leg, feet_current);
		
			if (verbose) {
				g_print("\nTaking step %d with leg %d\n", i, stepping_leg);
				g_print("current_cog: " TRIPLE_FORMAT "\n", ITEMS_OF(current_cog));
				g_print("centroid:    " TRIPLE_FORMAT "\n", ITEMS_OF(centroid));
			}

			for_each_axis(axis) {
				step_delta[axis] = foot_deltas[stepping_leg][axis];
				// Delta the foot is shifting
				shift_delta[axis] = SHIFT_AMOUNT * (current_cog[axis] - centroid[axis]);
				// Compensate step_delta for loss of distance because of the shift back:
				cumulative_shift_delta[axis] += shift_delta[axis];
			}

			if (verbose) {
				g_print("step_delta:             " TRIPLE_FORMAT "\n", ITEMS_OF(step_delta));
				g_print("shift_delta:            " TRIPLE_FORMAT "\n", ITEMS_OF(shift_delta));
				g_print("cumulative_shift_delta: " TRIPLE_FORMAT "\n", ITEMS_OF(cumulative_shift_delta));
			}
			
			// For each timestep.  TODO: Make timestep length variable based on step length.
			for (float t = 0.0; t < 1.01; t += (1.0 / NUM_TIMESTEPS)) {
				guint16 goal_pos[12];
				
				for_each_leg (leg) {
					float t_shift = 1 - exp(-15.0 * t);
					feet_current[leg][0] = feet_step_start[leg][0] + t_shift * shift_delta[0];
					feet_current[leg][1] = feet_step_start[leg][1] + t_shift * shift_delta[1];
					feet_current[leg][2] = feet_step_start[leg][2] + t_shift * shift_delta[2];
					if (leg == stepping_leg) {
						float t_step = t;// > 0.3 ? (t - 0.3) * (1.0 / 0.7) : 0;
						feet_current[leg][0] += step_delta[0] * t_step;
						feet_current[leg][1] += step_delta[1] * t_step;
						feet_current[leg][2] += step_delta[2] * t_step - sinf(t_step * G_PI) * STEP_HEIGHT;
					}
					
					kin_robot_coords_to_leg_coords(foot_leg_coords[leg], feet_current[leg], leg);
					kin_leg_coords_to_motor_angles(leg_angles[leg], foot_leg_coords[leg], leg);
					
					if (verbose) {
						g_print("Timestep %3.2f leg %d: " TRIPLE_FORMAT " cmR " TRIPLE_FORMAT " cmL -> " TRIPLE_FORMAT " rad\n",
								t, leg, ITEMS_OF(feet_current[leg]), ITEMS_OF(foot_leg_coords[leg]), ITEMS_OF(leg_angles[leg]));
					}

					for_each_joint (joint) {
						guint id = MOTOR_ID(leg, joint);
						goal_pos[id] = rx28_rad_to_motor_val(leg_angles[leg][joint]);
						RX28_MOTOR_MOVEMENT(mm[id], id, goal_pos[id], MOTOR_SPEED);
					}
				}

				if (enable_movement) {
					rx28_move_motors(mm, 12);
					rx28_poll_completion(goal_pos, last_goal_pos);
					//char temp;
					//scanf("%c", &temp);
				}
				for_each_motor (id) {
					last_goal_pos[id] = goal_pos[id];
				}
			}

			// Now we have angles, so we can use forward and calculate the CoG
			kin_center_of_gravity(current_cog, leg_angles);
			if (verbose) g_print("CoG: %3.2f %3.2f %3.2f cmR\n", ITEMS_OF(current_cog));
		}
	}

	do_stand();
	
	return 0;
}
