#include "include/raylib.h"
#include "include/raymath.h"
#include "include/rlgl.h"
#include <vector>

#include "util.hpp"
#include "cube.hpp"
#include "bsp.hpp"

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

std::vector<Triangle*> init_triangles() {
    return std::vector<Triangle*> {
        /* new Triangle({10.0f, -4.0f, -5.0f}, {10.0f, -4.0f, 5.0f}, {10.0f, 4.0f, -1.0f}), */
    };
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Multisampling 4x
    // Initialization
    //--------------------------------------------------------------------------------------
    std::vector<Triangle*> triangles = init_triangles();
    std::vector<Cube> cubes = init_cubes();
    for(auto& cube : cubes) {
        auto cube_triangles = cube.get_triangles();
        triangles.insert(triangles.end(), cube_triangles.begin(), cube_triangles.end());
    }

    // if index == -1 all triangles are visible
    int invisible_indx = -1;
    bool wire_mode = false;

    float zNear = 0.1f, zFar = 100.0f;
    const float default_fovy = 60.0f;
    float fovy = default_fovy;
    Matrix project_mat = get_project_matrix(screenWidth, screenHeight, fovy, zNear, zFar);
    
    Vector3 camera_up = (Vector3){0.0f, 1.0f, 0.0f};
    Vector3 camera_pos = (Vector3){0.0f, 0.0f, 0.0f};
    Vector3 camera_target = (Vector3){0.0f, 0.0f, -1.0f};
    Vcam camera = Vcam(camera_pos, camera_up, camera_target, project_mat);
    float mouse_sensitivity = 0.001f;

    Matrix move_right = MatrixIdentity();
    move_right.m12 = -0.1f;
    Matrix move_left = MatrixIdentity();
    move_left.m12 = 0.1f;
    Matrix move_up = MatrixIdentity();
    move_up.m13 = -0.1f;
    Matrix move_down = MatrixIdentity();
    move_down.m13 = 0.1f;
    Matrix move_forward = MatrixIdentity();
    move_forward.m14 = 0.1f;
    Matrix move_backward = MatrixIdentity();
    move_backward.m14 = -0.1f;

    BSPTree bsp_tree = BSPTree(triangles);
    
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
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera.get_up(), mouse_delta.x * mouse_sensitivity);
            Quaternion q_yaw_camera = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, -mouse_delta.x * mouse_sensitivity);

            Quaternion q_pitch_rot = QuaternionFromAxisAngle({1.0, 0.0, 0.0}, mouse_delta.y * mouse_sensitivity);
            Quaternion q_pitch_camera = QuaternionFromAxisAngle(camera.get_right(), -mouse_delta.y * mouse_sensitivity);
            camera.rotate_target(q_yaw_camera);
            camera.rotate_right(q_yaw_camera);

            camera.rotate_target(q_pitch_camera);
            camera.rotate_up(q_pitch_rot);

            for(auto& t : triangles) {
                t->rotate(q_yaw_rot);
                t->rotate(q_pitch_rot);
            }
        }

        if(IsKeyDown(KEY_H)) {
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera.get_up(), -7.0f * mouse_sensitivity);
            Quaternion q_yaw_camera = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, 7.0 * mouse_sensitivity);
            camera.rotate_target(q_yaw_camera);
            camera.rotate_right(q_yaw_camera);

            for(auto& t : triangles)
                t->rotate(q_yaw_rot);
        }

        if(IsKeyDown(KEY_L)) {
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera.get_up(), 7.0f * mouse_sensitivity);
            Quaternion q_yaw_camera = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, -7.0 * mouse_sensitivity);
            camera.rotate_target(q_yaw_camera);
            camera.rotate_right(q_yaw_camera);

            for(auto& t : triangles)
                t->rotate(q_yaw_rot);
        }

        if(IsKeyDown(KEY_J)) {
            Quaternion q_pitch_rot = QuaternionFromAxisAngle({1.0f, 0.0f, 0.0f}, 7.0f * mouse_sensitivity);
            Quaternion q_pitch_camera = QuaternionFromAxisAngle(camera.get_right(), -7.0f * mouse_sensitivity);
            camera.rotate_target(q_pitch_camera);
            camera.rotate_up(q_pitch_rot);

            for(auto& t : triangles)
                t->rotate(q_pitch_rot);
        }

        if(IsKeyDown(KEY_K)) {
            Quaternion q_pitch_rot = QuaternionFromAxisAngle({1.0f, 0.0f, 0.0f}, -7.0f * mouse_sensitivity);
            Quaternion q_pitch_camera = QuaternionFromAxisAngle(camera.get_right(), 7.0f * mouse_sensitivity);
            camera.rotate_target(q_pitch_camera);
            camera.rotate_up(q_pitch_rot);

            for(auto& t : triangles)
                t->rotate(q_pitch_rot);
        }

        if(IsKeyDown(KEY_Q)) {
            Quaternion q_roll_rot = QuaternionFromAxisAngle({0.0, 0.0, 1.0}, 10.0f * mouse_sensitivity);
            Quaternion q_roll_camera = QuaternionFromAxisAngle(camera.get_target(), -10.0f * mouse_sensitivity);
            camera.rotate_right(q_roll_camera);
            camera.rotate_up(q_roll_camera);

            for(auto& t : triangles) {
                t->rotate(q_roll_rot);
            }
        }

        if(IsKeyDown(KEY_E)) {
            Quaternion q_roll_rot = QuaternionFromAxisAngle({0.0, 0.0, 1.0}, -10.0f * mouse_sensitivity);    
            Quaternion q_roll_camera = QuaternionFromAxisAngle(camera.get_target(), 10.0f * mouse_sensitivity);    
            camera.rotate_right(q_roll_camera);
            camera.rotate_up(q_roll_camera);
            
            for(auto& t : triangles) {
                t->rotate(q_roll_rot);
            }
        }

        if(IsKeyDown(KEY_W)) {
            camera.move_forward();
            for(auto& t : triangles)
                t->multiply_by_matrix(move_forward);
        }

        if(IsKeyDown(KEY_S)) {
            camera.move_backward();
            for(auto& t : triangles)
                t->multiply_by_matrix(move_backward);
        }
        
        if(IsKeyDown(KEY_A)) {
            camera.move_left();
            for(auto& t : triangles)
                t->multiply_by_matrix(move_left);
        }
        
        if(IsKeyDown(KEY_D)) {
            camera.move_right();
            for(auto& t : triangles)
                t->multiply_by_matrix(move_right);
        }
        
        if(IsKeyDown(KEY_SPACE)) {
            camera.move_up();
            Matrix move_up = MatrixIdentity();
            move_up.m12 = camera.get_up().x * -0.1f;
            move_up.m13 = camera.get_up().y * -0.1f;
            move_up.m14 = camera.get_up().z * -0.1f;
            for(auto& t : triangles)
                t->multiply_by_matrix(move_up);
        }

        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.move_down();
            Matrix move_down = MatrixIdentity();
            move_down.m12 = camera.get_up().x * 0.1f;
            move_down.m13 = camera.get_up().y * 0.1f;
            move_down.m14 = camera.get_up().z * 0.1f;
            for(auto& t : triangles)
                t->multiply_by_matrix(move_down);
        }
        
        // wire mode
        if(IsKeyPressed(KEY_R)) {
            wire_mode ^= true;
            if(wire_mode) rlEnableWireMode();
            else rlDisableWireMode();
        }

        if(IsKeyPressed(KEY_V)) {
            if(invisible_indx != -1)
                triangles[invisible_indx]->visible = true;
            invisible_indx = invisible_indx == triangles.size() - 1 ? -1 : invisible_indx + 1;
            if(invisible_indx != -1)
                triangles[invisible_indx]->visible = false;
        }

        if(IsKeyDown(KEY_KP_ADD)) {
            if(fovy > 1.0f)
                fovy -= 0.1f;
            project_mat = get_project_matrix(screenWidth, screenHeight, fovy, zNear, zFar);
            camera.set_projection_mat(project_mat);
        }

        if(IsKeyDown(KEY_KP_SUBTRACT)) {
            if(fovy < default_fovy)
                fovy += 0.1f;
            project_mat = get_project_matrix(screenWidth, screenHeight, fovy, zNear, zFar);
            camera.set_projection_mat(project_mat);
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        
        bsp_tree.draw(camera);

        DrawCircle(screenWidth/2, screenHeight/2, 7.5f, RED);

        DrawText(TextFormat("Move: WSAD, SPACE, LCTRL"), 20, 20, 20, BLACK);
        DrawText(TextFormat("Rotate: Mouse (or HJKL), Q, E"), 20, 40, 20, BLACK);
        DrawText(TextFormat("Zoom: +, -"), 20, 60, 20, BLACK);
        DrawText(TextFormat("Zoom x%.2f", default_fovy/fovy), 20, 80, 20, BLACK);

        DrawText(TextFormat("Camera x target %f", camera.get_target().x), 20, 120, 20, BLACK);
        DrawText(TextFormat("Camera y target %f", camera.get_target().y), 20, 140, 20, BLACK);
        DrawText(TextFormat("Camera z target %f", camera.get_target().z), 20, 160, 20, BLACK);

        DrawText(TextFormat("Camera x up %f", camera.get_up().x), 20, 200, 20, BLACK);
        DrawText(TextFormat("Camera y up %f", camera.get_up().y), 20, 220, 20, BLACK);
        DrawText(TextFormat("Camera z up %f", camera.get_up().z), 20, 240, 20, BLACK);

        DrawText(TextFormat("Camera x right %f", camera.get_right().x), 20, 280, 20, BLACK);
        DrawText(TextFormat("Camera y right %f", camera.get_right().y), 20, 300, 20, BLACK);
        DrawText(TextFormat("Camera z right %f", camera.get_right().z), 20, 320, 20, BLACK);

        DrawText(TextFormat("Camera x pos %f", camera.get_pos().x), 20, 360, 20, BLACK);
        DrawText(TextFormat("Camera y pos %f", camera.get_pos().y), 20, 380, 20, BLACK);
        DrawText(TextFormat("Camera z pos %f", camera.get_pos().z), 20, 400, 20, BLACK);

        DrawText(TextFormat("Invisible triangle index %d", invisible_indx), 20, 440, 20, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    for(auto& t : triangles)
        delete t;

    return 0;
}

