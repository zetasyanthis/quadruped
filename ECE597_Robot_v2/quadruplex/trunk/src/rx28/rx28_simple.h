#define word guint16
#define byte guint8
#define RX28_MEM(addr, name, type, min, max, read_write) \
   gint32 rx28_get_##name(guint8 id); \
   gint32 rx28_set_##name(guint8 id, type val);
#include "rx28.def"
#undef RX28_MEM
#undef word
#undef byte


#define RX28_MOTOR_MOVEMENT(v, _id, _goal_pos, _speed) do { \
   (v).id = _id; \
   (v).goal_pos = _goal_pos; \
   (v).speed = _speed; \
} while (0)

struct rx28_motor_movement {
   guint8 id;
   guint16 goal_pos;
   guint16 speed;
};


/// Converts an angle provided by the neural network (in radians, float) to a motor angle value (0-1023, guint).
#define rx28_rad_to_motor_val(rad_angle) \
	({  \
		guint ang = (guint)(512.0 + (rad_angle*180.0/G_PI*(512.0/150.0))); \
		CLAMP(ang, 0, 1023); \
	})

/// Converts a motor angle value (0-1023) to an angle in radians (float).
#define rx28_motor_val_to_rad(motor_val) \
	({ \
		(((gint)motor_val) - 512.0) * (150.0/512.0) * (G_PI / 180.0); \
	})



void rx28_simple_init(void);
int rx28_move_motors(struct rx28_motor_movement *m, guint8 len);
guint16 rx28_calc_speed(guint16 present_pos, guint16 goal_pos, guint time_ms);
void rx28_poll_completion(guint16 goal_pos[12], guint16 last_goal_pos[12]);
void rx28_move(float leg_angles[4][3], guint time);
