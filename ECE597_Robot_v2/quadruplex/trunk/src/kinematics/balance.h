#define MASS_OF_ROBOT 5.165
#define MASS_OF_FOOT 0.105
#define MASS_OF_MOTOR 0.215
#define MASS_OF_LEG (2 * MASS_OF_MOTOR + MASS_OF_FOOT)
#define MASS_OF_MAIN_BODY (MASS_OF_ROBOT - 4 * MASS_OF_LEG)

void kin_center_of_gravity(float com[3], float motor_cog_positions[4][3]);
void kin_triangle_centroid(float centroid[3], guint foot_to_skip, float vertices[4][3]);
float kin_check_stability_cross(float foot_positions[4][3], float cog[3], int moving_leg);

