#include "util.hpp"
#include "include/raymath.h"
#include <cmath>
#include <cstdlib>
#include <cstdio>

Vcam::Vcam(Vector3 camera_pos, Vector3 camera_up, Vector3 camera_target, Matrix projection_matrix) {
    this->camera_up = camera_up;
    this->camera_pos = camera_pos;
    this->camera_target = camera_target;
    this->camera_right = Vector3CrossProduct(camera_target, camera_up);
    this->projection_matrix = projection_matrix;
}

void Vcam::move_forward() {
    camera_pos = Vector3Add(camera_pos, Vector3Scale(Vector3Normalize(camera_target), 0.1));
}

void Vcam::move_backward() {
    camera_pos = Vector3Subtract(camera_pos, Vector3Scale(Vector3Normalize(camera_target), 0.1));
}

void Vcam::move_left() {
    camera_pos = Vector3Subtract(camera_pos, Vector3Scale(Vector3Normalize(camera_right), 0.1));
}

void Vcam::move_right() {
    camera_pos = Vector3Add(camera_pos, Vector3Scale(Vector3Normalize(camera_right), 0.1));
}

void Vcam::move_up() {
    camera_pos = Vector3Add(camera_pos, Vector3Scale((Vector3){0.0f, 1.0f, 0.0f}, 0.1f));
}

void Vcam::move_down() {
    camera_pos = Vector3Subtract(camera_pos, Vector3Scale((Vector3){0.0f, 1.0f, 0.0f}, 0.1f));
}

void Vcam::rotate_up(Quaternion q) {
    camera_up = Vector3RotateByQuaternion(camera_up, q);
}

void Vcam::rotate_right(Quaternion q) {
    camera_right = Vector3RotateByQuaternion(camera_right, q);
}

void Vcam::rotate_target(Quaternion q) {
    camera_target = Vector3RotateByQuaternion(camera_target, q);
}

void Vcam::set_projection_mat(Matrix projection_mat) {
    this->projection_matrix = projection_mat;
}

Matrix Vcam::get_project_mat() const {
    return projection_matrix;
}

Vector3 Vcam::get_up() const {
    return camera_up;
}

Vector3 Vcam::get_target() const {
    return camera_target;
}

Vector3 Vcam::get_right() const {
    return camera_right;
}

Vector3 Vcam::get_pos() const {
    return camera_pos;
}

Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3) {
    this->verticies[0] = v1;
    this->verticies[1] = v2;
    this->verticies[2] = v3;
    this->color = get_random_color();
    this->visible = true;
}

Triangle::Triangle(Vector3 v1, Vector3 v2, Vector3 v3, Color color) {
    this->verticies[0] = v1;
    this->verticies[1] = v2;
    this->verticies[2] = v3;
    this->color = color;
    this->visible = true;
}

// does triangle plane cross given triangle
// 1 if in front, 0 if they cross, -1 if behind
int Triangle::plane_cross_triangle(Triangle* triangle) const {
    Vector4 plane = to_plane();
    float sign_p1 = point_in_plane_equasion(triangle->verticies[0], plane);
    float sign_p2 = point_in_plane_equasion(triangle->verticies[1], plane);
    float sign_p3 = point_in_plane_equasion(triangle->verticies[2], plane);
    if(sign_p1 == 0 && sign_p2 == 0 && sign_p3 == 0)
        return 1;
    else if((sign_p1 >= 0 && sign_p2 >= 0 && sign_p3 >= 0))
        return 1;
    else if(sign_p1 <= 0 && sign_p2 <= 0 && sign_p3 <= 0)
        return -1;

    return 0;
}

Vector4 Triangle::to_plane() const {
    auto v1 = Vector3Subtract(verticies[0], verticies[1]);
    auto v2 = Vector3Subtract(verticies[0], verticies[2]);
    auto normal = Vector3CrossProduct(v1, v2);
    auto point = verticies[0];
    auto d = -normal.x * point.x -normal.y * point.y - normal.z * point.z;

    return (Vector4){normal.x, normal.y, normal.z, d};
}

void Triangle::rotate(Quaternion& q) {
    for(int i = 0; i < 3; i++)
        verticies[i] = Vector3RotateByQuaternion(verticies[i], q);
}

void Triangle::multiply_by_matrix(Matrix& mat) {
    for(auto& v : verticies)
        v = multiply_mv(mat, v);
}

void Triangle::draw(const Vcam& camera) const {
    if(!this->visible)
        return;

    Vector3 projected_verticies[3] = {0};
    Vector2 projected_screen_verticies[3] = {0};
    
    for(int i = 0; i < 3; i++) {
        projected_verticies[i] = multiply_mv(camera.get_project_mat(), verticies[i]);
        projected_screen_verticies[i] = get_2d_screen_vec(projected_verticies[i]);
    }

    // double draw to omit clock wise triangles only
    if(z_in_range(projected_verticies[0].z) && z_in_range(projected_verticies[1].z && z_in_range(projected_verticies[2].z)))
        DrawTriangle(projected_screen_verticies[0], projected_screen_verticies[1], projected_screen_verticies[2], color);
    if(z_in_range(projected_verticies[2].z) && z_in_range(projected_verticies[1].z && z_in_range(projected_verticies[0].z)))
        DrawTriangle(projected_screen_verticies[2], projected_screen_verticies[1], projected_screen_verticies[0], color);
}

bool Triangle::is_visible() const {
    return visible;
}

Triangle Triangle::copy() const {
    return Triangle(verticies[0], verticies[1], verticies[2], color);
}

Vector3 multiply_mv(const Matrix &mat, const Vector3 &vec) {
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

Vector2 get_2d_screen_vec(const Vector3& vec) {
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

Color get_random_color() {
    unsigned char rand_r = (float)rand()/(float)RAND_MAX * 255;
    unsigned char rand_g = (float)rand()/(float)RAND_MAX * 255;
    unsigned char rand_b = (float)rand()/(float)RAND_MAX * 255;
    return (Color){rand_r, rand_g, rand_b, 255};
}

void print_matrix(const Matrix& mat) {
    printf("%f %f %f %f\n", mat.m0, mat.m4, mat.m8, mat.m12);
    printf("%f %f %f %f\n", mat.m1, mat.m5, mat.m9, mat.m13);
    printf("%f %f %f %f\n", mat.m2, mat.m6, mat.m10, mat.m14);
    printf("%f %f %f %f\n", mat.m3, mat.m7, mat.m11, mat.m15);
    printf("\n");
}

bool z_in_range(float z) {
    return z > 0 && z <= 1;
}

float point_in_plane_equasion(Vector3 point, Vector4 plane) {
    return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
}

Matrix get_project_matrix(int screenWidth, int screenHeight, float fovy, float zNear, float zFar) {
    Matrix project_mat = {0};
    float aspect = (float)screenWidth/screenHeight;
    fovy = fovy*DEG2RAD;
    float f = 1.0f/tan(fovy/2);

    project_mat.m0 = f/aspect;
    project_mat.m5 = -f;
    project_mat.m10 = (zFar+zNear)/(zNear-zFar);
    project_mat.m11 = -1;
    project_mat.m14 = 2*zFar*zNear/(zNear-zFar);

    return project_mat;
}

