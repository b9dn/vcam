#ifndef CUBE_HPP
#define CUBE_HPP

#include "include/raylib.h"

class Cube {
private:
    Vector3 center;
    float side_size;
    Vector3 verticies[8];
    int triangles[12*3];
    Color colors[12];

    bool z_in_range(float z);

    void set_verticies(Vector3 center);

    void set_triangles();

    void set_colors();

public:
    Cube(Vector3 center, float side_size);

    void rotate(Quaternion& q);

    void draw(Matrix& project_matrix);

    Vector3 get_center();

    void multiply_by_matrix(Matrix& mat);
};

#endif
