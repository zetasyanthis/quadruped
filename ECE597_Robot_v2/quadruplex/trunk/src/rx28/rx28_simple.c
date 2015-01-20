#include <glib.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include <helper.h>
#include <rx28/rx28.h>

#include "rx28_simple.h"


static gboolean rx28_simple_initialized = FALSE;
static guint16 last_goal_pos[12] = {0};


/// Initialize the rx28_simple internal state by polling the motors
/// for their present position.
void
rx28_simple_init(void)
{
	g_assert(!rx28_simple_initialized);
	
	for_each_motor (id) {
		gint32 pos = rx28_get_present_position(id);
		if (pos < 0) {
			g_printerr("Error getting position of motor %d", id);
			last_goal_pos[id] = 512;
		} else {
			last_goal_pos[id] = pos;
		}
	}
	
	rx28_simple_initialized = TRUE;
}


#define word guint16
#define byte guint8
#define RX28_MEM(addr, name, type, min, max, read_write) \
   gint32 rx28_get_##name(guint8 id)\
   { \
      type tmp; \
      if (rx28_read_data_##type(id, addr, &tmp)) \
         return -1; \
      return tmp; \
   } \
   gint32 rx28_set_##name(guint8 id, type val) \
   { \
      if (!read_write) return -1; \
      return rx28_write_data_##type(id, addr, val); \
   }
#include "rx28.def"

#undef RX28_MEM
#undef word
#undef byte


/// Move len motors simultaneously with the specified speed
int
rx28_move_motors(struct rx28_motor_movement *m, guint8 len)
{
   struct sync_write sw[len];
   for (gint i = 0; i < len; i++) {
      sw[i].id = m[i].id;
      sw[i].data = malloc(4);
      PACK_U16(sw[i].data, m[i].goal_pos);
      PACK_U16(sw[i].data+2, m[i].speed);
   }

   rx28_sync_write(kRX28GoalPosition, sw, len, 4);

   for (gint i = 0; i < len; i++) free(sw[i].data);

   return 0;
}


/// Calculate the servo speed necessary to traverse the specified angle in the specified duration.
guint16
rx28_calc_speed(guint16 present_pos, guint16 goal_pos, guint time_ms)
{
	gint32 delta = ABS((gint32) goal_pos - (gint32) present_pos);
	float revs_to_turn = delta * RX28_ANGLE_UNITS_TO_REVOLUTIONS;
	float minutes_to_turn = time_ms / (1000.0 * 60.0); // convert to minutes
	float speed = CLAMP((revs_to_turn /  minutes_to_turn) * RX28_RPM_TO_SPEED_UNITS, 1, 1023);
//	g_print("present_pos = %u, goal_pos = %u, delta = %d, revs_to_turn = %f, minutes_to_turn = %f, speed = %f, retval = %d\n",
//			present_pos, goal_pos, delta, revs_to_turn, minutes_to_turn, speed, (guint16) speed);
	return roundf(speed);
}


void
rx28_poll_completion(guint16 goal_pos[12], guint16 last_goal_pos[12])
{
	gint max = 0;
	gint max_id = 0;
	
	for_each_motor (i) {
		gint delta = (gint)goal_pos[i] - (gint)last_goal_pos[i];
		//g_print("delta for motor %2d is %d\n", i, delta);
		gint diff = ABS(delta);
		if (diff > max) {
			max = diff;
			max_id = i;
		}
	}
	//g_print("polling for completion on motor %2d with delta %d", max_id, max);

	if (max < 1) {
		//g_print("\n");
		return;
	}
	
	while (TRUE) {
		gint pos = rx28_get_present_position(max_id);
		gint goal = goal_pos[max_id];
		if (ABS(pos - goal) < 10 || !rx28_get_moving(max_id)) {
			break;
		}
		//g_print(".");
		usleep(10000);
	}
	//g_print("\n");

}

/// Move the motors to the specified goal positions in `time` milliseconds.
/// rx28_simple_init() must be called before this function.
void
rx28_move(float leg_angles[4][3], guint time)
{
	g_assert(rx28_simple_initialized);
	
	guint16 goal_pos[12];
	struct rx28_motor_movement mm[12];
	
	for_each_leg_joint (leg, joint, id) {
		goal_pos[id] = rx28_rad_to_motor_val(leg_angles[leg][joint]);
		guint16 speed = rx28_calc_speed(last_goal_pos[id], goal_pos[id], time);
		if (id == 0) {
			//g_print("motor %d speed is %u\n", id, speed);
		}
		RX28_MOTOR_MOVEMENT(mm[id], id, goal_pos[id], speed);
	}
	
	rx28_move_motors(mm, 12);
	rx28_poll_completion(goal_pos, last_goal_pos);
	
	memcpy(last_goal_pos, goal_pos, 12 * sizeof(guint16));
}

