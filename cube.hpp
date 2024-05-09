#ifndef CUBE_HPP
#define CUBE_HPP

#include <vector>
#include "include/raylib.h"
#include "util.hpp"

class Cube {
private:
    Vector3 center;
    float side_size;
    Vector3 verticies[8];
    int triangles[12*3];
    Color colors[12];

    bool z_in_range(float z) const;

    void set_verticies(Vector3 center);

    void set_triangles();

    void set_colors();

public:
    Cube(Vector3 center, float side_size);

    void rotate(Quaternion& q);

    void draw(Matrix& project_matrix) const;

    Vector3 get_center() const;

    std::vector<Triangle*> get_triangles() const;

    void multiply_by_matrix(Matrix& mat);
};

#endif
