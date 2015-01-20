// Function prototypes

void nn_activate(float input[3], float output[3]);

void kin_robot_coords_to_leg_coords(float leg[], float robot[], guint leg_number);
void kin_leg_coords_to_robot_coords(float robot[3], float leg[3], guint leg_number);
void kin_leg_coords_to_motor_angles(float angles[3], float leg_coords[3], guint leg_number);
void kin_right_front_angles(float xyz_in[], float theta_out[]);
void kin_left_front_angles(float xyz_in[], float theta_out[]);
void kin_right_back_angles(float xyz_in[], float theta_out[]);
void kin_left_back_angles(float xyz_in[], float theta_out[]);

