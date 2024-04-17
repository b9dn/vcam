#ifndef UTIL_HPP
#define UTIL_HPP

#include "include/raylib.h"

const int screenWidth = 1500;
const int screenHeight = 900;

Vector3 multiply_mv(Matrix& mat, Vector3& vec);

Vector2 get_2d_screen_vec(Vector3& vec);

Vector3 get_random_vector(float min, float max);

void print_matrix(Matrix mat);

Matrix get_project_matrix(int screenWidth, int screenHeight, float fovy, float zNear, float zFar);

#endif
