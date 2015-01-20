// Macros

/// Add two 3-length vectors.
#define ADD_VECTORS(out, v1, v2) \
   do { \
      out[0] = v1[0] + v2[0]; \
      out[1] = v1[1] + v2[1]; \
      out[2] = v1[2] + v2[2]; \
   } while (0);

/// Subtract two 3-length vectors.
#define SUBTRACT_VECTORS(out, v1, v2) \
   do { \
      out[0] = v1[0] - v2[0]; \
      out[1] = v1[1] - v2[1]; \
      out[2] = v1[2] - v2[2]; \
   } while (0);

// Function Prototypes
void kin_print_mat(guint rows, guint cols, float mat[rows][cols]);
void kin_print_vect(guint dim, float vect[dim]);
void kin_fill_rx(float mat[3][3], float theta);
void kin_fill_ry(float mat[3][3], float theta);
void kin_fill_rz(float mat[3][3], float theta);
void kin_mat_product(float out[3][3], float left[3][3], float right[3][3]);
void kin_mat_vect_product(float out[3], float matrix[3][3], const float vector[3]);

