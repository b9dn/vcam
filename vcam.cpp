#include "include/raylib.h"
#include "include/raymath.h"
#include "include/rlgl.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "util.hpp"
#include "cube.hpp"

std::vector<Cube> init_cubes() {
    return std::vector<Cube> {
        Cube({0.0f, 0.0f, -10.0f}, 1.0f), 
        Cube({4.0f, 0.0f, -10.0f}, 1.0f),
        Cube({-4.0f, 0.0f, -10.0f}, 1.0f),
        Cube({0.0f, 0.0f, -14.0f}, 1.0f), 
        Cube({4.0f, 0.0f, -14.0f}, 1.0f),
        Cube({-4.0f, 0.0f, -14.0f}, 1.0f),
        Cube({0.0f, 0.0f, -18.0f}, 1.0f), 
        Cube({4.0f, 0.0f, -18.0f}, 1.0f),
        Cube({-4.0f, 0.0f, -18.0f}, 1.0f),

        Cube({0.0f, -4.0f, -10.0f}, 1.0f), 
        Cube({4.0f, -4.0f, -10.0f}, 1.0f),
        Cube({-4.0f, -4.0f, -10.0f}, 1.0f),
        Cube({0.0f, -4.0f, -14.0f}, 1.0f), 
        Cube({4.0f, -4.0f, -14.0f}, 1.0f),
        Cube({-4.0f, -4.0f, -14.0f}, 1.0f),
        Cube({0.0f, -4.0f, -18.0f}, 1.0f), 
        Cube({4.0f, -4.0f, -18.0f}, 1.0f),
        Cube({-4.0f, -4.0f, -18.0f}, 1.0f),

        Cube({0.0f, 4.0f, -10.0f}, 1.0f), 
        Cube({4.0f, 4.0f, -10.0f}, 1.0f),
        Cube({-4.0f, 4.0f, -10.0f}, 1.0f),
        Cube({0.0f, 4.0f, -14.0f}, 1.0f), 
        Cube({4.0f, 4.0f, -14.0f}, 1.0f),
        Cube({-4.0f, 4.0f, -14.0f}, 1.0f),
        Cube({0.0f, 4.0f, -18.0f}, 1.0f), 
        Cube({4.0f, 4.0f, -18.0f}, 1.0f),
        Cube({-4.0f, 4.0f, -18.0f}, 1.0f),
    };
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Ustawienie multisamplingu na 4x
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(NULL));

    float side_size = 1.0f;
    /* int cubes_n = 9; */
    std::vector<Cube> cubes = init_cubes();

    /* for(int i = 0; i < cubes_n; i++) { */
    /*     Vector3 cube_center = get_random_vector(-10.0f, 10.0f); */
    /*     cube_center.y = 0.0f; */
    /*     cubes.push_back(Cube(cube_center, side_size)); */
    /* } */

    float zNear = 0.1f, zFar = 100.0f;
    const float default_fovy = 60.0f;
    float fovy = default_fovy;
    Matrix project_mat = get_project_matrix(screenWidth, screenHeight, fovy, zNear, zFar);
    
    Vector3 camera_up = (Vector3){0.0f, 1.0f, 0.0f};
    float mouse_sensitivity = 0.001f;

    Matrix move_right = MatrixIdentity();
    move_right.m12 = -0.1f;
    Matrix move_left = MatrixIdentity();
    move_left.m12 = 0.1f;
    Matrix move_up = MatrixIdentity();
    move_up.m13 = 0.1f;
    Matrix move_down = MatrixIdentity();
    move_down.m13 = -0.1f;
    Matrix move_forward = MatrixIdentity();
    move_forward.m14 = 0.1f;
    Matrix move_backward = MatrixIdentity();
    move_backward.m14 = -0.1f;
    
    InitWindow(screenWidth, screenHeight, "Virtual camera");

    SetTargetFPS(60);
    DisableCursor();
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        
        Vector2 mouse_delta = GetMouseDelta();
        if(mouse_delta.x != 0 || mouse_delta.y != 0) {
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera_up, mouse_delta.x * mouse_sensitivity);
            Quaternion q_pitch_rot = QuaternionFromAxisAngle({-1.0, 0.0, 0.0}, mouse_delta.y * mouse_sensitivity);
            camera_up = Vector3RotateByQuaternion(camera_up, q_pitch_rot);

            for(auto& c : cubes) {
                c.rotate(q_yaw_rot);
                c.rotate(q_pitch_rot);
            }
        }
        if(IsKeyDown(KEY_Q)) {
            Quaternion q_roll_rot = QuaternionFromAxisAngle({0.0, 0.0, 1.0}, 10.0f * mouse_sensitivity);
            for(auto& c : cubes) {
                c.rotate(q_roll_rot);
            }
        }
        if(IsKeyDown(KEY_E)) {
            Quaternion q_roll_rot = QuaternionFromAxisAngle({0.0, 0.0, -1.0}, 10.0f * mouse_sensitivity);    
            for(auto& c : cubes) {
                c.rotate(q_roll_rot);
            }
        }

        if(IsKeyDown(KEY_W)) {
            for(auto& c : cubes)
                c.multiply_by_matrix(move_forward);
        }
        if(IsKeyDown(KEY_S)) {
            for(auto& c : cubes)
                c.multiply_by_matrix(move_backward);
        }
        if(IsKeyDown(KEY_A)) {
            for(auto& c : cubes)
                c.multiply_by_matrix(move_left);
        }
        if(IsKeyDown(KEY_D)) {
            for(auto& c : cubes)
                c.multiply_by_matrix(move_right);
        }
        if(IsKeyDown(KEY_SPACE)) {
            for(auto& c : cubes)
                c.multiply_by_matrix(move_up);
        }
        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            for(auto& c : cubes)
                c.multiply_by_matrix(move_down);
        }

        if(IsKeyDown(KEY_KP_ADD)) {
            if(fovy > 1.0f)
                fovy -= 0.1f;
            project_mat = get_project_matrix(screenWidth, screenHeight, fovy, zNear, zFar);
        }
        if(IsKeyDown(KEY_KP_SUBTRACT)) {
            if(fovy < default_fovy)
                fovy += 0.1f;
            project_mat = get_project_matrix(screenWidth, screenHeight, fovy, zNear, zFar);
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(TextFormat("Move: WSAD, SPACE, LCTRL"), 20, 20, 20, BLACK);
        DrawText(TextFormat("Rotate: Mouse, Q, E"), 20, 40, 20, BLACK);
        DrawText(TextFormat("Zoom: +, -"), 20, 60, 20, BLACK);
        DrawText(TextFormat("Zoom x%.2f", default_fovy/fovy), 20, 80, 20, BLACK);
        
        for(auto& cube : cubes)
            cube.draw(project_mat);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

