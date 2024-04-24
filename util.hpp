#ifndef UTIL_HPP
#define UTIL_HPP

#include "include/raylib.h"

const int screenWidth = 1500;
const int screenHeight = 900;

Vector3 multiply_mv(const Matrix& mat, const Vector3& vec);

Vector2 get_2d_screen_vec(Vector3& vec);

Vector3 get_random_vector(float min, float max);

Color get_random_color();

void print_matrix(Matrix mat);

bool z_in_range(float z);

float point_in_plane_equasion(Vector3 point, Vector4 plane);

Matrix get_project_matrix(int screenWidth, int screenHeight, float fovy, float zNear, float zFar);

#endif
