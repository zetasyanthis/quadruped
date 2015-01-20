// Autogenerated file: use src/kinematics/leg_defs.py to edit

/// The xyz position change from the shoulder joint pivot point to the knee joint pivot point.
static const float s1[3] = {4.5, 0.0, 2.5};
 /// The xyz position change from the knee joint pivot point to the foot joint pivot point.
static const float s2[3] = {0.0, 0.0, 6.7};
/// The xyz position change from the foot joint pivot point to the tip of the foot.
static const float s3[3] = {0.0, 9.75, 3.25};

/// The xyz position change from the shoulder joint pivot point to the cog of the knee motor.
static const float s1_cog[3] = {4.2,0.0,1.0};
/// The xyz position change from the knee joint pivot point to the cog of the foot motor.
static const float s2_cog[3] = {0.0,0.0,5.0};
/// The xyz position change from the foot joint pivot point to the cog of foot.
static const float s3_cog[3] = {0.0,4.8,3.0};

/// Offset from the center of the robot to the feet.
static const float leg_offsets[3] = {8.8 / 2.0, (18.35 / 2.0) + 1.5, 0.0};