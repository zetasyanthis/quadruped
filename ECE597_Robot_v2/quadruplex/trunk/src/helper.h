// Note to self: DO NOT ABUSE "global" HELPERS

// Macros

#define for_each_axis(i) \
	for (guint (i) = 0; (i) < 3; (i)++)

#define for_each_motor(i) \
	for (guint (i) = 0; (i) < 12; (i)++)

#define for_each_leg(i) \
	for (guint (i) = 0; (i) < 4; (i)++)

#define for_each_joint(i) \
	for (guint (i) = 0; (i) < 3; (i)++)

#define for_each_leg_joint(leg, joint, motor) \
	for (guint (leg) = 0, (motor) = 0; (leg) < 4; (leg)++) \
	for (guint (joint) = 0; (joint) < 3; (joint)++, (motor)++)

#define MOTOR_ID(leg, joint) ((leg) * 3 + (joint))


