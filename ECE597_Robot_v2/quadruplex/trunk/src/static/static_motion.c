#include <glib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <rx28/rs485.h>
#include <rx28/rx28.h>
#include <rx28/rx28_simple.h>

#include "static_motion.h"

gint
static_playback_motion_array(const guint8 motions[][4])
{	
	guint i = 0; // Index in motions
	guint16 goal_pos[12];
	guint16 present_pos[12];
	guint16 delta[12];
	guint16 max_delta = 0;
	struct rx28_motor_movement mm[12];

	while (motions[i][0] != 0xFF) { // Iterate over each burst
		guint j = 0; // Index in current burst
		do {
			guint motor_id = motions[i + j][1];
			
			goal_pos[j] = (motions[i + j][3] << 8) | motions[i + j][2];
			rx28_read_data_word(motor_id, kRX28PresentPosition, &(present_pos[j]));
			delta[j] = ABS((gint) goal_pos[j] - (gint) present_pos[j]);
			if (delta[j] > max_delta) {
				max_delta = delta[j];
			}
			j++;
			g_print("i=%u, j=%u, motor_id=%u delta=%u, max_delta=%u\n", i, j, motor_id, delta[j], max_delta);
		} while (motions[i + j][0] == 0);
		
		// Set the speeds
		for (guint k = 0; k < j; k++) {
			guint motor_id = motions[i + k][1];
			//guint16 speed = CLAMP((guint16) (1023.0 * (gfloat) delta[k] / (gfloat) max_delta), 1, 1023);
			guint16 speed = rx28_calc_speed(present_pos[k], goal_pos[k], 500);
			g_print("burst %u, motor %u: goal_pos=%u, speed=%u\n",
					i, motor_id, goal_pos[k], speed);

			RX28_MOTOR_MOVEMENT(mm[motor_id], motor_id, goal_pos[k], speed);
		}

		rx28_move_motors(mm, 12);
		rx28_poll_completion(goal_pos, present_pos);
		
		i += j;
	}
	return 0;
}

