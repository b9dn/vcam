#ifndef UTIL_HPP
#define UTIL_HPP

#include "include/raylib.h"

const int screenWidth = 1500;
const int screenHeight = 900;

class Vcam {
public:
    Vector3 camera_up;
    Vector3 camera_pos;
    Vector3 camera_target;
    Vector3 camera_right;
    Matrix projection_matrix;

    Vcam(Vector3 camera_pos, Vector3 camera_up, Vector3 camera_target, Matrix projection_matrix);
};

class Triangle {
public:
    Vector3 verticies[3];
    Color color;
    bool visible;
    Triangle(Vector3 v1, Vector3 v2, Vector3 v3, Color color);

    Triangle(Vector3 v1, Vector3 v2, Vector3 v3);

    // does triangle plane cross given triangle
    // 1 if in front, 0 if they cross, -1 if behind
    int plane_cross_triangle(Triangle* triangle);

    Vector4 to_plane();

    void rotate(Quaternion& q);

    void multiply_by_matrix(Matrix& mat);

    void draw(const Vcam& camera);
};

Vector3 multiply_mv(const Matrix& mat, const Vector3& vec);

Vector2 get_2d_screen_vec(Vector3& vec);

Vector3 get_random_vector(float min, float max);

Color get_random_color();

void print_matrix(Matrix mat);

bool z_in_range(float z);

float point_in_plane_equasion(Vector3 point, Vector4 plane);

Matrix get_project_matrix(int screenWidth, int screenHeight, float fovy, float zNear, float zFar);

#endif
