#include "util.hpp"
#include <cmath>
#include <cstdlib>
#include <cstdio>

Vector3 multiply_mv(Matrix &mat, Vector3 &vec) {
    Vector3 ret;
    Vector4 result;

    result.x = mat.m0*vec.x + mat.m4*vec.y + mat.m8*vec.z + mat.m12;
    result.y = mat.m1*vec.x + mat.m5*vec.y + mat.m9*vec.z + mat.m13;
    result.z = mat.m2*vec.x + mat.m6*vec.y + mat.m10*vec.z + mat.m14;
    result.w = mat.m3*vec.x + mat.m7*vec.y + mat.m11*vec.z + mat.m15;

    ret.x = result.x/result.w;
    ret.y = result.y/result.w;
    ret.z = result.z/result.w;

    return ret;
}

Vector2 get_2d_screen_vec(Vector3& vec) {
    return (Vector2){
        (vec.x/vec.z + 1.0f) * screenWidth/2.0f, 
        (vec.y/vec.z + 1.0f) * screenHeight/2.0f
    };
}

Vector3 get_random_vector(float min, float max) {
    float x = (float)rand()/(float)RAND_MAX * (max - min) + min;
    float y = (float)rand()/(float)RAND_MAX * (max - min) + min;
    float z = (float)rand()/(float)RAND_MAX * (max - min) + min;
    return (Vector3){ x, y, z };
}

void print_matrix(Matrix mat) {
    printf("%f %f %f %f\n", mat.m0, mat.m4, mat.m8, mat.m12);
    printf("%f %f %f %f\n", mat.m1, mat.m5, mat.m9, mat.m13);
    printf("%f %f %f %f\n", mat.m2, mat.m6, mat.m10, mat.m14);
    printf("%f %f %f %f\n", mat.m3, mat.m7, mat.m11, mat.m15);
    printf("\n");
}

Matrix get_project_matrix(int screenWidth, int screenHeight, float fovy, float zNear, float zFar) {
    Matrix project_mat = {0};
    float aspect = (float)screenWidth/screenHeight;
    fovy = fovy*DEG2RAD;
    float f = 1.0f/tan(fovy/2);

    project_mat.m0 = f/aspect;
    project_mat.m5 = f;
    project_mat.m10 = (zFar+zNear)/(zNear-zFar);
    project_mat.m11 = -1;
    project_mat.m14 = 2*zFar*zNear/(zNear-zFar);

    return project_mat;
}

