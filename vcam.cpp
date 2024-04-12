#include "include/raylib.h"
#include "include/raymath.h"
#include "include/rlgl.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

const int screenWidth = 1500;
const int screenHeight = 900;

Vector3 multiply_mv(Matrix& mat, Vector3& vec) {
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

Vector2 project_vertex(Vector3 vertex, Matrix project_mat) {
    Vector3 projected = multiply_mv(project_mat, vertex);

    return (Vector2){
        (projected.x/projected.z + 1.0f) * screenWidth/2.0f, 
        (projected.y/projected.z + 1.0f) * screenHeight/2.0f
    };
}

class Cube {
private:
    Vector3 center;
    float side_size;
    Vector3 verticies[8];

public:
    Cube(Vector3 center, float side_size) {
        this->center = center;
        this->side_size = side_size;
        
        set_center(center);
    }

    void rotate(Quaternion q) {
        for(int i = 0; i < 8; i++)
            verticies[i] = Vector3RotateByQuaternion(verticies[i], q);
    }

    void draw(Matrix& project_matrix) {
        Vector2 projected_verticies[8] = {0};
        
        for(int i = 0; i < 8; i++)
            projected_verticies[i] = project_vertex(verticies[i], project_matrix);

        for(int i = 0; i < 4; i++) {
            DrawLine(projected_verticies[i].x, projected_verticies[i].y, projected_verticies[(i+1) % 4].x, projected_verticies[(i+1) % 4].y, BLACK);
            DrawLine(projected_verticies[i + 4].x, projected_verticies[i + 4].y, projected_verticies[((i+1) % 4) + 4].x, projected_verticies[((i+1) % 4) + 4].y, BLACK);
            DrawLine(projected_verticies[i].x, projected_verticies[i].y, projected_verticies[i + 4].x, projected_verticies[i + 4].y, BLACK);
        }
    }

    void set_center(Vector3 center) {
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

    Vector3 get_center() {
        return center;
    }

    void multiply_by_matrix(Matrix mat) {
        for(auto& v : verticies)
            v = multiply_mv(mat, v);
    }
};

Vector3 get_random_vector(float min, float max) {
    float x = (float)rand()/(float)RAND_MAX * (max - min) + min;
    float y = (float)rand()/(float)RAND_MAX * (max - min) + min;
    float z = (float)rand()/(float)RAND_MAX * (max - min) + min;
    /* return (Vector3){ x, y, z }; */
    return (Vector3){ 0.0f, 0.0f, -10.0f };
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
    fovy = 60.0*DEG2RAD;
    float f = 1.0f/tan(fovy/2);

    project_mat.m0 = f/aspect;
    project_mat.m5 = f;
    project_mat.m10 = (zFar+zNear)/(zNear-zFar);
    project_mat.m11 = -1;
    project_mat.m14 = 2*zFar*zNear/(zNear-zFar);

    return project_mat;
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Ustawienie multisamplingu na 4x
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(NULL));

    float side_size = 1.0f;
    int cubes_n = 1;
    std::vector<Cube> cubes;
    for(int i = 0; i < cubes_n; i++) {
        Vector3 cube_center = get_random_vector(-10.0f, 10.0f);
        cube_center.y = 0.0f;
        cubes.push_back(Cube(cube_center, side_size));
    }

    float zNear = 0.1f, zFar = 100.0f;
    Matrix project_mat = get_project_matrix(screenWidth, screenHeight, 60.0f, zNear, zFar);
    Matrix translation_matrix = MatrixIdentity();
    Quaternion rot = QuaternionIdentity();
    
    Vector3 camera_up = (Vector3){0.0f, 1.0f, 0.0f};
    Vector3 camera_target = (Vector3){0.0f, 0.0f, -1.0f};
    float mouse_sensitivity = 0.001f;
    
    InitWindow(screenWidth, screenHeight, "raylib");

    SetTargetFPS(60);
    DisableCursor();
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        
        Vector2 mouse_delta = GetMouseDelta();
        if(mouse_delta.x != 0 || mouse_delta.y != 0) {
            Quaternion q_x_rot = QuaternionFromAxisAngle(camera_up, mouse_delta.x * mouse_sensitivity);
            Quaternion q_y_rot = QuaternionFromAxisAngle({-1.0, 0.0, 0.0}, mouse_delta.y * mouse_sensitivity);
            camera_up = Vector3RotateByQuaternion(camera_up, q_y_rot);

            for(auto& c : cubes) {
                c.rotate(q_x_rot);
                c.rotate(q_y_rot);
            }
        }
        if(IsKeyDown(KEY_Q)) {
            Quaternion q_z_rot = QuaternionFromAxisAngle({0.0, 0.0, 1.0}, 10.0f * mouse_sensitivity);
            for(auto& c : cubes) {
                c.rotate(q_z_rot);
            }
        }
        if(IsKeyDown(KEY_E)) {
            Quaternion q_z_rot = QuaternionFromAxisAngle({0.0, 0.0, -1.0}, 10.0f * mouse_sensitivity);    
            for(auto& c : cubes) {
                c.rotate(q_z_rot);
            }
        }


        /* } */
        /* if(IsKeyDown(KEY_S)) { */
        /*     translation_mat.m14 -= 0.1f; */
        /* } */
        /* if(IsKeyDown(KEY_A)) { */
        /*     translation_mat.m12 += 0.1f; */
        /* } */
        /* if(IsKeyDown(KEY_D)) { */
        /*     translation_mat.m12 -= 0.1f; */
        /* } */
        if(IsKeyDown(KEY_W)) {
            Matrix m = MatrixIdentity();
            m.m14 = 0.1f;
            for(auto& c : cubes)
                c.multiply_by_matrix(m);
        }
        if(IsKeyDown(KEY_S)) {
            Matrix m = MatrixIdentity();
            m.m14 = -0.1f;
            for(auto& c : cubes)
                c.multiply_by_matrix(m);
        }
        if(IsKeyDown(KEY_A)) {
            Matrix m = MatrixIdentity();
            m.m12 = 0.1f;
            for(auto& c : cubes)
                c.multiply_by_matrix(m);
        }
        if(IsKeyDown(KEY_D)) {
            Matrix m = MatrixIdentity();
            m.m12 = -0.1f;
            for(auto& c : cubes)
                c.multiply_by_matrix(m);
        }
        /* if(IsKeyDown(KEY_B)) { */
        /*     /1* b = true; *1/ */
        /*     /1* printf("Breakpoint\n"); *1/ */
        /* } */

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        for(auto& cube : cubes) {
            cube.draw(project_mat);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

