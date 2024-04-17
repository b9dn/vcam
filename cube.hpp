#ifndef CUBE_HPP
#define CUBE_HPP

#include "include/raylib.h"

class Cube {
private:
    Vector3 center;
    float side_size;
    Vector3 verticies[8];

    bool z_in_range(float z);

public:
    Cube(Vector3 center, float side_size);

    void rotate(Quaternion& q);

    void draw(Matrix& project_matrix);

    void set_center(Vector3 center);

    Vector3 get_center();

    void multiply_by_matrix(Matrix& mat);
};

#endif
