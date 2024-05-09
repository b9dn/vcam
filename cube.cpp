#include "include/raylib.h"
#include "util.hpp"
#include "cube.hpp"
#include "include/raymath.h"
#include <vector>

bool Cube::z_in_range(float z) const {
    return z > 0 && z <= 1;
}

Cube::Cube(Vector3 center, float side_size) {
    this->center = center;
    this->side_size = side_size;
    
    set_verticies(center);
    set_triangles();
    set_colors();
}

void Cube::rotate(Quaternion& q) {
    for(int i = 0; i < 8; i++)
        verticies[i] = Vector3RotateByQuaternion(verticies[i], q);
}

void Cube::draw(Matrix& project_matrix) const {
    Vector3 projected_verticies[8] = {0};
    Vector2 projected_screen_verticies[8] = {0};
    
    for(int i = 0; i < 8; i++) {
        projected_verticies[i] = multiply_mv(project_matrix, verticies[i]);
        projected_screen_verticies[i] = get_2d_screen_vec(projected_verticies[i]);
    }

    for(int i = 0; i < 36; i+=3) {
        // double draw to ommit clock wise triangle drawing
        if(z_in_range(projected_verticies[triangles[i]].z) && 
            z_in_range(projected_verticies[triangles[i+1]].z &&
                z_in_range(projected_verticies[triangles[i+2]].z)))

            DrawTriangle(
                projected_screen_verticies[triangles[i]],
                projected_screen_verticies[triangles[i+1]],
                projected_screen_verticies[triangles[i+2]],
                colors[i/3]
            );

        if(z_in_range(projected_verticies[triangles[35-i]].z) &&
            z_in_range(projected_verticies[triangles[35-i-1]].z &&
                z_in_range(projected_verticies[triangles[35-i-2]].z)))

            DrawTriangle(
                projected_screen_verticies[triangles[35-i]],
                projected_screen_verticies[triangles[35-i-1]],
                projected_screen_verticies[triangles[35-i-2]],
                colors[12-i/3]
            );
    }
}

void Cube::set_verticies(Vector3 center) {
    this->center = center;

    verticies[0] = (Vector3){center.x-side_size, center.y-side_size, center.z-side_size};
    verticies[1] = (Vector3){center.x+side_size, center.y-side_size, center.z-side_size};
    verticies[2] = (Vector3){center.x+side_size, center.y+side_size, center.z-side_size};
    verticies[3] = (Vector3){center.x-side_size, center.y+side_size, center.z-side_size};

    verticies[4] = (Vector3){center.x-side_size, center.y-side_size, center.z+side_size};
    verticies[5] = (Vector3){center.x+side_size, center.y-side_size, center.z+side_size}; 
    verticies[6] = (Vector3){center.x+side_size, center.y+side_size, center.z+side_size}; 
    verticies[7] = (Vector3){center.x-side_size, center.y+side_size, center.z+side_size};
}

void Cube::set_triangles() {
    triangles[0] = 0; triangles[1] = 1; triangles[2] = 2;
    triangles[3] = 2; triangles[4] = 3; triangles[5] = 0;

    triangles[6] = 1; triangles[7] = 5; triangles[8] = 6;
    triangles[9] = 6; triangles[10] = 2; triangles[11] = 1;

    triangles[12] = 4; triangles[13] = 5; triangles[14] = 6;
    triangles[15] = 6; triangles[16] = 7; triangles[17] = 4;

    triangles[18] = 0; triangles[19] = 4; triangles[20] = 7;
    triangles[21] = 7; triangles[22] = 3; triangles[23] = 0;

    triangles[24] = 3; triangles[25] = 2; triangles[26] = 6;
    triangles[27] = 6; triangles[28] = 7; triangles[29] = 3;

    triangles[30] = 0; triangles[31] = 1; triangles[32] = 5;
    triangles[33] = 5; triangles[34] = 4; triangles[35] = 0;
}

std::vector<Triangle*> Cube::get_triangles() const {
    std::vector<Triangle*> return_triangles;
    for(int i = 0; i < 36; i += 3)
        return_triangles.push_back(
            new Triangle(verticies[triangles[i]], verticies[triangles[i+1]], verticies[triangles[i+2]])
        );

    return return_triangles;
}

void Cube::set_colors() {
    for(int i = 0; i < 12; i++)
        colors[i] = get_random_color();
}

Vector3 Cube::get_center() const {
    return center;
}

void Cube::multiply_by_matrix(Matrix& mat) {
    for(auto& v : verticies)
        v = multiply_mv(mat, v);
}

