#include "cube.hpp"
#include "util.hpp"
#include "include/raymath.h"

bool Cube::z_in_range(float z) {
    return z > 0 && z <= 1;
}

Cube::Cube(Vector3 center, float side_size) {
    this->center = center;
    this->side_size = side_size;
    
    set_center(center);
}

void Cube::rotate(Quaternion& q) {
    for(int i = 0; i < 8; i++)
        verticies[i] = Vector3RotateByQuaternion(verticies[i], q);
}

void Cube::draw(Matrix& project_matrix) {
    Vector3 projected_verticies[8] = {0};
    Vector2 projected_screen_verticies[8] = {8};
    
    for(int i = 0; i < 8; i++) {
        projected_verticies[i] = multiply_mv(project_matrix, verticies[i]);
        projected_screen_verticies[i] = get_2d_screen_vec(projected_verticies[i]);
    }

    for(int i = 0; i < 4; i++) {
        if(z_in_range(projected_verticies[i].z) && z_in_range(projected_verticies[(i+1) % 4].z))
            DrawLineV(projected_screen_verticies[i], projected_screen_verticies[(i+1) % 4], BLACK);

        if(z_in_range(projected_verticies[i + 4].z) && z_in_range(projected_verticies[((i+1) % 4) + 4].z))
            DrawLineV(projected_screen_verticies[i + 4], projected_screen_verticies[((i+1) % 4) + 4], BLACK);
        
        if(z_in_range(projected_verticies[i].z) && z_in_range(projected_verticies[i + 4].z))
            DrawLineV(projected_screen_verticies[i], projected_screen_verticies[i + 4], BLACK);
    }
}

void Cube::set_center(Vector3 center) {
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

Vector3 Cube::get_center() {
    return center;
}

void Cube::multiply_by_matrix(Matrix& mat) {
    for(auto& v : verticies)
        v = multiply_mv(mat, v);
}

