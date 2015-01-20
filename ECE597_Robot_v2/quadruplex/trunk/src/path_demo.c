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
#include <sensors/i2c_bus.h>
#include <sensors/srf08.h>

/// Unpack the three elements of foo, comma-separated.  Useful for passing to g_print()
#define ITEMS_OF(foo) foo[0], foo[1], foo[2]
/// Format for a tuple of three floating-point numbers.
#define TRIPLE_FORMAT "%+9.4f %+9.4f %+9.4f"

#define NUM_TIMESTEPS 10
#define TIMESTEP_MS   50
/// Distance to shift the body in each timestep, in centimeters
#define QUAD_SHIFT_TIMESTEP_DIST .5

#define MAX_ANGLE (30 * (G_PI / 180.0))
#define MAX_DISTANCE 3.0

#define PAUSE ({                                            \
    if (pause) {                                            \
        g_print("Press enter to continue... ");             \
        getc(stdin);                                        \
    }                                                       \
})

/// Set the feet to go to the xyz positions (in robot coordinates) specified in
/// feet_start.  Speed is the raw motor speed to perform the action.
static void 
do_movement(float feet[4][3], guint16 speed, gboolean verbose)
{
	float leg_coords[3];
	float leg_angles[4][3];

	for_each_leg (leg) {
		kin_robot_coords_to_leg_coords(leg_coords, feet[leg], leg);
		kin_leg_coords_to_motor_angles(leg_angles[leg], leg_coords, leg);
        if (verbose) {
            g_print("Leg %d: %+8.4f %+8.4f %+8.4f cm -> %+8.4f %+8.4f %+8.4f rad\n",
                    leg, ITEMS_OF(leg_coords), ITEMS_OF(leg_angles[leg]));
        }
	}

	rx28_move(leg_angles, speed);
}

/// Apply 2D rotation and translation to the array of xyz locations in current_pos.
static void
fill_foot_deltas(float foot_deltas[4][3], float current_pos[4][3], float distance, float angle)
{
	// Take in an angle between -30 and 30 degrees (in radians) and 

	float translated_pos[3];
	float rotated_pos[4][3];
	float rotation[3][3];
	float rot_angle = G_PI_2 + CLAMP(angle, -MAX_ANGLE, MAX_ANGLE);
	kin_fill_rz(rotation, angle);
	
	float translation[3] = {0, distance * sinf(rot_angle), 0};
	
	for_each_leg (leg) {
		ADD_VECTORS(translated_pos, current_pos[leg], translation);
		kin_mat_vect_product(rotated_pos[leg], rotation, translated_pos);
	}

	for_each_leg (leg) {
		SUBTRACT_VECTORS(foot_deltas[leg], rotated_pos[leg], current_pos[leg]);
	}
}

static int
choose_leg(int step_index, float distance, float angle)
{
	//return step_index;
    if (distance >= 0) {
        if (angle >= 0.0) {
            switch (step_index) {
                case 0:
                    return 0;
                case 1:
                    return 2;
                case 2:
                    return 1;
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
    } else {
        if (angle >= 0.0) {
            switch (step_index) {
                case 0:
                    return 3;
                case 1:
                    return 1;
                case 2:
                    return 2;
                case 3:
                    return 0;
            }
        } else {
            switch (step_index) {
                case 0:
                    return 2;
                case 1:
                    return 0;
                case 2:
                    return 3;
                case 3:
                    return 1;
            }
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

	gboolean verbose = FALSE;
	gboolean wiimote_control = FALSE;
    gboolean last_stand = FALSE;
    gboolean pause = FALSE;
    double step_height_double = 5.0;
    double stand_height = 12.0;
	gboolean autonomous = FALSE;
	
	GError *option_error = NULL;
	GOptionEntry option_entries[] = {
		{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Display messages", NULL },
		{ "wiimote-control", 'w', 0, G_OPTION_ARG_NONE, &wiimote_control, "Enable Wiimote control", NULL },
        { "custers-last-stand", 's', 0, G_OPTION_ARG_NONE, &last_stand, "Perform a stand at the end of the program", NULL },
        { "pause", 'p', 0, G_OPTION_ARG_NONE, &pause, "Pause in places", NULL },
        { "step-height", 'h', 0, G_OPTION_ARG_DOUBLE, &step_height_double, "Height to raise the feet", NULL },
        { "stand-height", 'H', 0, G_OPTION_ARG_DOUBLE, &stand_height, "Height of the default stand", NULL },
        { "autonomous", 'a', 0, G_OPTION_ARG_NONE, &autonomous, "Goes on a rampage", NULL },
		{ NULL },
	};
	GOptionContext *option_context = g_option_context_new("- walk robot in pre-programmed sequence");
	g_option_context_add_main_entries(option_context, option_entries, NULL);
	if (!g_option_context_parse(option_context, &argc, &argv, &option_error)) {
		g_printerr("Error parsing options: %s\n", option_error->message);
		exit(2);
	}
	g_option_context_free(option_context);

    if (wiimote_control && autonomous) {
        g_printerr("Wiimote control and autonomous navigation are mutually exclusive\n");
        exit(2);
    }
	
    if (wiimote_control) {
        wiimote_init();
    }

	if (rs485_open("/dev/ttyS1") < 0) {
		g_error("couldn't open serial port");
	}
	rx28_simple_init();

    float step_height = (float) step_height_double;
	//float foot_start_coords[3] = {8.5, 1.5, 15}; // Starting position of each leg
	float foot_start_coords[3] = {8.5, 1.5, stand_height}; // Starting position of each leg
	float feet_start [4][3]; // Robot coordinates of where the feet are now
	float foot_deltas[4][3]; // Robot coordinates of where the feet should go, relative to starting position

	// Convert the starting leg coordinates to the robot's coordinate frame
	if (verbose) g_print("Initial foot positions\n");
	for_each_leg (i) {
		kin_leg_coords_to_robot_coords(feet_start[i], foot_start_coords, i);
		if (verbose) {
            g_print("%d starting at %+9.4f %+9.4f %+9.4f cmL\n", i, ITEMS_OF(foot_start_coords));
            g_print("%d starting at %+9.4f %+9.4f %+9.4f cmR\n", i, ITEMS_OF(feet_start[i]));
        }
	}

	// Status variables
	float feet_current[4][3]; // Current XYZ positions of the feet in robot coordinates
	float leg_angles[4][3]; // Angles of leg joints in radians
	float current_cog[3] = {0.0, 0.0, 0.0}; // We assume that it starts at the origin
	float centroid[3] = {0.0, 0.0, 0.0};
	float shift_delta[3] = {0.0, 0.0, 0.0};

	void do_stand()
	{
		if (verbose) g_print("\nAssuming standing position\n");
		do_movement(feet_start, 600, verbose);
		for_each_leg_joint (leg, joint, id) {
			feet_current[leg][joint] = feet_start[leg][joint];
		}
		centroid[0] = 0;
		centroid[1] = 0;
		centroid[2] = 0;
		current_cog[0] = 0;
		current_cog[1] = 0;
		current_cog[2] = 0;
	}

	void four_steps(float distance, float angle)
	{
		g_print("Four steps: distance %.2f cm, %.2f rad\n", distance, angle);

		// Generate a delta for each foot
		fill_foot_deltas(foot_deltas, feet_start, distance, angle);
		if (verbose) {
			g_print("\nFoot deltas:\n");
			for_each_leg (i) {
				g_print("%d delta is %+9.4f %+9.4f %+9.4f\n", i, ITEMS_OF(foot_deltas[i]));
			}
		}

		for (guint i = 0; i < 4; i++) {
			float step_delta[3];
			guint stepping_leg = choose_leg(i, distance, angle);
			kin_triangle_centroid(centroid, stepping_leg, feet_current);
		
			if (verbose) {
				g_print("\nTaking step %d with leg %d\n", i, stepping_leg);
				g_print("current_cog:   " TRIPLE_FORMAT "\n", ITEMS_OF(current_cog));
				g_print("centroid:      " TRIPLE_FORMAT "\n", ITEMS_OF(centroid));
			}


			for_each_axis(axis) {
				// Delta the feet need to move relative to start
				step_delta[axis] = foot_deltas[stepping_leg][axis];
				// Distance to the center of the stability triangle
				// We move along this vector in the quad shift phase until we reach
				// the edge of the stability triangle.  The amount moved in the quad
				// shift phase (`shift_made`) is subtracted from shift_delta, and
				// the movement remaining in shift_delta is spread out over the course
				// of the step.
				shift_delta[axis] = current_cog[axis] - centroid[axis];
			}
			float shift_len = sqrtf(shift_delta[0] * shift_delta[0] + shift_delta[1] * shift_delta[1]);
			float shift_increment[3] = {
				shift_delta[0] / shift_len * QUAD_SHIFT_TIMESTEP_DIST,
                shift_delta[1] / shift_len * QUAD_SHIFT_TIMESTEP_DIST,
				0
			};
			/// Total shift completed during "quad shift" phase
			float shift_made[3] = {0.0, 0.0, 0.0};

			if (verbose) {
				g_print("step_delta:      " TRIPLE_FORMAT "\n", ITEMS_OF(step_delta));
				g_print("shift_delta:     " TRIPLE_FORMAT "\n", ITEMS_OF(shift_delta));
				g_print("shift_increment: " TRIPLE_FORMAT "\n", ITEMS_OF(shift_increment));
			}

			if (verbose) g_print("\nStarting quad shift phase\n");
            PAUSE;

			// Shift the CoG toward the stability triangle until we cross the edge
			float dist;
			while ((dist = kin_check_stability_cross(feet_current, current_cog, stepping_leg)) <= 50.0) {
				if (verbose) g_print("quad shift: dist = %+9.4f\n", dist);
				for_each_leg (leg) {
					ADD_VECTORS(feet_current[leg], feet_current[leg], shift_increment);

					// Convert global positions to angles
					float foot_leg_coords[3];
					kin_robot_coords_to_leg_coords(foot_leg_coords, feet_current[leg], leg);
					kin_leg_coords_to_motor_angles(leg_angles[leg], foot_leg_coords, leg);
				}
				rx28_move(leg_angles, TIMESTEP_MS);
				ADD_VECTORS(shift_made, shift_made, shift_increment);
				kin_center_of_gravity(current_cog, leg_angles);
			}
			if (verbose) {
				g_print("shift_made:  " TRIPLE_FORMAT " cm\n", ITEMS_OF(shift_made));
			}

			// Compensate for the change in our position
			//ADD_VECTORS(step_delta, step_delta, shift_made);
			SUBTRACT_VECTORS(shift_delta, shift_delta, shift_made);

			// All movements are relative to the position at the start of the step
			float feet_step_start[4][3]; // XYZ positions of the feet at the start of the step
			for_each_leg_joint (leg, joint, id) {
				feet_step_start[leg][joint] = feet_current[leg][joint];
			}

			if (verbose) {
				g_print("\nStarting step\n");
                PAUSE;
				g_print("feet_step_start:\n");
				for_each_leg (leg) {
					g_print("leg %d: " TRIPLE_FORMAT "\n", leg, ITEMS_OF(feet_current[leg]));
				}
				g_print("step_delta:  " TRIPLE_FORMAT "\n", ITEMS_OF(step_delta));
				g_print("shift_delta: " TRIPLE_FORMAT "\n", ITEMS_OF(shift_delta));
				g_print("CoG:         " TRIPLE_FORMAT " cmR\n", ITEMS_OF(current_cog));
			}

			// For each timestep.  TODO: Make timestep length variable based on step length.
			for (float t = 0.0; t < 1.01; t += (1.0 / NUM_TIMESTEPS)) {
				if (verbose) g_print("timestep %.4f\n", t);
				for_each_leg (leg) {
					feet_current[leg][0] = feet_step_start[leg][0];
					feet_current[leg][1] = feet_step_start[leg][1];
					feet_current[leg][2] = feet_step_start[leg][2];
					if (leg == stepping_leg) {
                        feet_current[leg][0] += (t * shift_delta[0]) + step_delta[0] * t;
                        feet_current[leg][1] += (t * shift_delta[1]) + step_delta[1] * t;
                        feet_current[leg][2] += (t * shift_delta[2]) + step_delta[2] * t - sinf(t * G_PI) * step_height;
					} else {
						feet_current[leg][0] += (t * shift_delta[0]);
						feet_current[leg][1] += (t * shift_delta[1]);
						feet_current[leg][2] += (t * shift_delta[2]);
					}

					if (verbose) {
						g_print("leg %d:        " TRIPLE_FORMAT " cmR\n", leg, ITEMS_OF(feet_current[leg]));
					}
					
					float foot_leg_coords[3];
					kin_robot_coords_to_leg_coords(foot_leg_coords, feet_current[leg], leg);
					if (verbose) {
						g_print("leg %d:        " TRIPLE_FORMAT " cmL\n", leg, ITEMS_OF(foot_leg_coords));
					}
					kin_leg_coords_to_motor_angles(leg_angles[leg], foot_leg_coords, leg);
				}

				rx28_move(leg_angles, TIMESTEP_MS);
				kin_center_of_gravity(current_cog, leg_angles);
				if (verbose) {
					g_print("CoG:         " TRIPLE_FORMAT " cmR\n", ITEMS_OF(current_cog));
				}
			}
		}
	}


    if (wiimote_control) {
        enum wiimote_conn_status connected = WIIMOTE_CONN_NONE;
        struct cwiid_nunchuk_mesg nunchuk = {0};
    
        while (TRUE) {
            wiimote_get_data(&connected, NULL, &nunchuk);
            if (connected != WIIMOTE_CONN_WIIMOTE) {
                sleep(1);
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
            //g_print("Wiimote directions: %+1.2f rad %+1.2f distance\n", angle, distance * MAX_DISTANCE);

            if (distance < 0.1) {
                //g_print("Distance near zero or negative: standing\n");
                do_stand();
                usleep(10000);
            } else {
            	usleep(500000);

            	wiimote_get_data(&connected, NULL, &nunchuk);
            	x = CLAMP((nunchuk.stick[0] - 122) / 100.0, -1.0, 1.0);
            	y = CLAMP((nunchuk.stick[1] - 133) / 100.0, -1.0, 1.0);
				angle = atan2f(x, y);
				angle = CLAMP(angle, -G_PI/12, G_PI/12);
	    	    distance = sqrtf(x*x + y*y);
	        	distance = CLAMP(distance, 0, 1.0);
            	if (angle > G_PI_2 || angle < -G_PI_2) distance = -distance;
            	g_print("Wiimote directions: %+1.2f rad %+1.2f distance\n", angle, distance * MAX_DISTANCE);

                four_steps(distance, angle);
            }
        }
	} else if (autonomous) {
		char *dev_filename = "/dev/i2c-2";
	    I2CBus *bus = i2c_bus_open(dev_filename);
	    SRF08 *sonar = srf08_new(bus, -1, -1, -1);
		
		i2c_bus_start_thread(bus);

		while(TRUE) {	
            enum {
                STATE_NOT_TURNING,
                STATE_TURN_LEFT,
                STATE_TURN_RIGHT
            } state = STATE_NOT_TURNING;
			gint light;
	        guint16 echoes[SRF08_NUM_ECHOES];
	        if (srf08_get_data(sonar, &light, echoes)) {
	            g_print("Sonic rangefinder primary echo distance:  %4u\n",echoes[0]);
                int distance = (int) echoes[0];
				if (distance > 60) {
                    state = STATE_NOT_TURNING;
					g_print("No object sighted\n");
					four_steps(3.0, 0);
				} else if (distance < 20) {
                    state = STATE_NOT_TURNING;
                    g_print("Backing up to avoid object at %d cm\n", distance);
                    four_steps(-4.0, 0.0);
                } else {
                    if (state == STATE_NOT_TURNING) {
                        state = (random() < (RAND_MAX / 2)) ? STATE_TURN_LEFT : STATE_TURN_RIGHT;
                    }
                    if (state == STATE_TURN_LEFT) {
                        g_print("Turning left to avoid object within %d cm\n", distance);
                        four_steps(1.0, G_PI / 12.0);
                    } else {
                        g_print("Turning right to avoid object within %d cm\n", distance);
                        four_steps(1.0, -G_PI / 12.0);
                    }
					do_stand();
				}
	        } else {
            	g_print("No sonic rangefinder data yet\n");
				do_stand();
        	}
		}
		
    } else {
        do_stand();
        four_steps(3.0, 0);
        four_steps(3.0, 0);
        four_steps(3.0, 0);
        four_steps(0.0, G_PI / 12.0);
    }
    if (last_stand) {
        PAUSE;
        do_stand();
    }
	
	return 0;
}

