#include <glib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <helper.h>

#include "kin_helper.h"

/**	Prints a matrix of size rows x cols
*	
*	\param rows				Number of rows
*	\param cols				Number of columns
*	\param mat	Matrix to be printed
*/
void
kin_print_mat(guint rows, guint cols, float mat[rows][cols])
{
	for (guint r = 0; r < rows; r++) {
		for (guint c = 0; c < cols; c++) {
			g_print("%s% 4.3f", !c ? "" : " ", mat[r][c]);
		}
		g_print("\n");
	}
}

/**	Prints a vector of length dim
*	
*	\param dim			Dimension of vector to be printed
*	\param vect			Vector to be printed
*/
void
kin_print_vect(guint dim, float vect[dim])
{
	for (guint i = 0; i < dim; i++) {
		g_print("%4.3f", vect[i]);
		g_print("\n");
	}
}


/** Fill the 3x3 matrix `mat` with values for a rotation matrix R_x(theta)
*	
*	\param	mat		Filled by function.  Contains the rotation matrix.
*	\param	theta	Angle of rotation.
*/
void
kin_fill_rx(float mat[3][3], float theta)
{
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[1][0] = 0;
	mat[1][1] = cosf(theta);
	mat[1][2] = -sinf(theta);
	mat[2][0] = 0;
	mat[2][1] = sinf(theta);
	mat[2][2] = cosf(theta);
	
	//sincosf(theta, &mat[2][1], &mat[2][2]);
	
	//mat[1][1] = mat[2][2];
	//mat[1][2] = -mat[2][1];
	//mat[2][0] = 0;
}


/** Fill an 3x3 matrix with values for a rotation matrix R_y(theta)
*
*	\param	mat		Filled by function.  Contains the rotation matrix.
*	\param	theta	Angle of rotation.
*/
void
kin_fill_ry(float mat[3][3], float theta)
{
	//sincosf(theta, &mat[0][2], &mat[0][0]);
	mat[0][0] = cosf(theta);
	mat[0][1] = 0;
	mat[0][2] = sinf(theta);

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;

	mat[2][0] = -sinf(theta);
	mat[2][1] = 0;
	mat[2][2] = cosf(theta);
	
	//mat[2][0] = -mat[0][2];
	//mat[2][1] = 0;
	//mat[2][2] = mat[0][0];
}


/** Fill an 3x3 matrix with values for a rotation matrix R_z(theta), which
* rotates the x-axis towards the y-axis.
*
*	\param	mat		Filled by function.  Contains the rotation matrix.
*	\param	theta	Angle of rotation.
*/
void
kin_fill_rz(float mat[3][3], float theta)
{
	mat[0][0] = cosf(theta);
	mat[0][1] = -sinf(theta);
	mat[0][2] = 0;

	mat[1][0] = sinf(theta);
	mat[1][1] = cosf(theta);;
	mat[1][2] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
}


/** Perform matrix multiplication between two 3x3 matrices. 
*	
*	\param out		Filled by function.  Result of matrix multiplication
*	\param left		Matrix on left of multiplication
*	\param right	Matrix on right of multiplication
*/
void
kin_mat_product(float out[3][3], float left[3][3], float right[3][3])
{
	for (guint i = 0; i < 3; i++) {
		for (guint j = 0; j < 3; j++) {
			out[i][j] = 0;
		}
	}

	for (guint left_row = 0; left_row < 3; left_row++) {
		for (guint right_col = 0; right_col < 3; right_col++) {
			for (guint i = 0; i < 3; i++) {
				out[left_row][right_col] += left[left_row][i] * right[i][right_col];
			}
		}
	}
}


/**	Calculates the product of a 3x3 matrix and a 3-length vector.  
*	\note [Matrix][Vector] is the order.
*	
*	\param matrix	Matrix to be multiplied.
*	\param vector	Vector to be multiplied.
*
*	\param out		Filled by function.  The 3-length product.
*/
void
kin_mat_vect_product(float out[3], float matrix[3][3], const float vector[3]) {
	for (guint i = 0; i < 3; i++) {
		out[i] = 0;
	}

	for (guint i = 0; i < 3; i++) {
		for (guint j = 0; j < 3; j++) {
			out[i] += matrix[i][j] * vector[j];
		}
	}
}

