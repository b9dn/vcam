#include "include/raylib.h"
#include "include/raymath.h"
#include "include/rlgl.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

#include "util.hpp"
#include "cube.hpp"

class Vcam {
public:
    Vector3 camera_up;
    Vector3 camera_pos;
    Vector3 camera_target;
    Vector3 camera_right;
    Matrix projection_matrix;

    Vcam(Vector3 camera_pos, Vector3 camera_up, Vector3 camera_target, Matrix projection_matrix) {
        this->camera_up = camera_up;
        this->camera_pos = camera_pos;
        this->camera_target = camera_target;
        this->camera_right = Vector3CrossProduct(camera_target, camera_up);
        this->projection_matrix = projection_matrix;
    }
};

class Triangle {
public:
    Vector3 verticies[3];
    Color color;
    Triangle(Vector3 v1, Vector3 v2, Vector3 v3, Color color) {
        this->verticies[0] = v1;
        this->verticies[1] = v2;
        this->verticies[2] = v3;
        this->color = color;
    }

    Triangle(Vector3 v1, Vector3 v2, Vector3 v3) {
        this->verticies[0] = v1;
        this->verticies[1] = v2;
        this->verticies[2] = v3;
        this->color = get_random_color();
    }

    // does triangle plane cross given triangle
    // 1 if in front, 0 if they cross, -1 if behind
    int plane_cross_triangle(Triangle* triangle) {
        auto v1 = Vector3Subtract(triangle->verticies[0], triangle->verticies[1]);
        auto v2 = Vector3Subtract(triangle->verticies[0], triangle->verticies[2]);
        auto normal = Vector3CrossProduct(v1, v2);
        auto point = triangle->verticies[0];
        auto d = -normal.x * point.x -normal.y * point.y - normal.z * point.z;

        Vector4 plane = (Vector4){normal.x, normal.y, normal.z, d};
        float sign_p1 = point_in_plane_equasion(triangle->verticies[0], plane);
        float sign_p2 = point_in_plane_equasion(triangle->verticies[1], plane);
        float sign_p3 = point_in_plane_equasion(triangle->verticies[2], plane);
        if((sign_p1 >= 0 && sign_p2 >= 0 && sign_p3 >= 0))
            return 1;
        else if(sign_p1 <= 0 && sign_p2 <= 0 && sign_p3 <= 0)
            return -1;

        return 0;
    }


    void rotate(Quaternion& q) {
        for(int i = 0; i < 3; i++)
            verticies[i] = Vector3RotateByQuaternion(verticies[i], q);
    }

    void multiply_by_matrix(Matrix& mat) {
        for(auto& v : verticies)
            v = multiply_mv(mat, v);
    }

    void draw(const Vcam& camera) {
        Vector3 projected_verticies[3] = {0};
        Vector2 projected_screen_verticies[3] = {0};
        
        for(int i = 0; i < 3; i++) {
            projected_verticies[i] = multiply_mv(camera.projection_matrix, verticies[i]);
            projected_screen_verticies[i] = get_2d_screen_vec(projected_verticies[i]);
        }

        // double draw to omit clock wise triangles only
        if(z_in_range(projected_verticies[0].z) && z_in_range(projected_verticies[1].z && z_in_range(projected_verticies[2].z)))
            DrawTriangle(projected_screen_verticies[0], projected_screen_verticies[1], projected_screen_verticies[2], color);
        if(z_in_range(projected_verticies[2].z) && z_in_range(projected_verticies[1].z && z_in_range(projected_verticies[0].z)))
            DrawTriangle(projected_screen_verticies[2], projected_screen_verticies[1], projected_screen_verticies[0], color);
    }
};

class BSPNode {
public:
    Triangle* triangle;
    Vector3 triangle_normal;
    Vector3 point_on_triangle;
    BSPNode* behind;
    BSPNode* front;

    BSPNode(Triangle* triangle) {
        this->triangle = triangle;
        this->point_on_triangle = triangle->verticies[0];
        this->behind = NULL;
        this->front = NULL;

        auto v1 = Vector3Subtract(triangle->verticies[0], triangle->verticies[1]);
        auto v2 = Vector3Subtract(triangle->verticies[0], triangle->verticies[2]);
        this->triangle_normal = Vector3CrossProduct(v1, v2);
    }

    bool camera_in_front(const Vcam& camera) {
        auto camera_vector = Vector3Subtract(camera.camera_pos, point_on_triangle);
        return Vector3DotProduct(camera_vector, triangle_normal) > 0 ? true : false;
    }
};

class BSPTree {
private:
    BSPNode* root;
    int size;

    // split triangle using plane
    // returns one of made front or behind triangles
    Triangle* split(Triangle* t, std::vector<Triangle*>& triangles, bool return_front) { ///////////////////
        return NULL;
    }

    Triangle* find_front(Triangle* triangle, std::vector<Triangle*>& triangles) {
        int i = 0;
        while(i++ < triangles.size()) {
            auto t = triangles[i];
            auto test_result = triangle->plane_cross_triangle(t);
            if(test_result == 1) {
                return t;
            }
            else if(test_result == 0) {
                return split(t, triangles, true);
            }
        }
        return NULL;
    }

    Triangle* find_behind(Triangle* triangle, std::vector<Triangle*>& triangles) {
        int i = 0;
        while(i++ < triangles.size()) {
            auto t = triangles[i];
            auto test_result = triangle->plane_cross_triangle(t);
            if(test_result == -1) {
                return t;
            }
            else if(test_result == 0) {
                return split(t, triangles, false);
            }
        }
        return NULL;
    }

    void make_bsp_tree(BSPNode* node, std::vector<Triangle*>& triangles) {
        if(triangles.size() <= 0 || node == NULL)
            return;

        Triangle* behid_triangle = find_behind(node->triangle, triangles);
        Triangle* front_triangle = find_front(node->triangle, triangles);

        if(behid_triangle != NULL) {
            triangles.erase(std::remove(triangles.begin(), triangles.end(), behid_triangle), triangles.end());
            node->behind = new BSPNode(behid_triangle);
        }
        if(front_triangle != NULL) {
            triangles.erase(std::remove(triangles.begin(), triangles.end(), front_triangle), triangles.end());
            node->front = new BSPNode(front_triangle);
        }

        make_bsp_tree(node->behind, triangles);
        make_bsp_tree(node->front, triangles);
    }

    void draw(const Vcam& camera, BSPNode* node) {
        if(node == NULL)
            return;

        auto is_camera_front = node->camera_in_front(camera);
        if(is_camera_front) {
            draw(camera, node->behind);
            node->triangle->draw(camera);
            draw(camera, node->front);
        }
        else {
            draw(camera, node->front);
            node->triangle->draw(camera);
            draw(camera, node->behind);
        }
    }

public:
    BSPTree(std::vector<Triangle*> triangles) {
        this->size = triangles.size();
        if(triangles.size() > 0) {
            root = new BSPNode(triangles[0]);
            triangles.erase(triangles.begin());
        }
        make_bsp_tree(root, triangles);
    }

    void draw(Vcam camera) {
        draw(camera, root);
    }
};

std::vector<Cube> init_cubes() {
    return std::vector<Cube> {
        Cube({0.0f, 0.0f, -10.0f}, 1.0f), 
        /* Cube({4.0f, 0.0f, -10.0f}, 1.0f), */
        /* Cube({-4.0f, 0.0f, -10.0f}, 1.0f), */
        /* Cube({0.0f, 0.0f, -14.0f}, 1.0f), */ 
        /* Cube({4.0f, 0.0f, -14.0f}, 1.0f), */
        /* Cube({-4.0f, 0.0f, -14.0f}, 1.0f), */
        /* Cube({0.0f, 0.0f, -18.0f}, 1.0f), */ 
        /* Cube({4.0f, 0.0f, -18.0f}, 1.0f), */
        /* Cube({-4.0f, 0.0f, -18.0f}, 1.0f), */

        /* Cube({0.0f, -4.0f, -10.0f}, 1.0f), */ 
        /* Cube({4.0f, -4.0f, -10.0f}, 1.0f), */
        /* Cube({-4.0f, -4.0f, -10.0f}, 1.0f), */
        /* Cube({0.0f, -4.0f, -14.0f}, 1.0f), */ 
        /* Cube({4.0f, -4.0f, -14.0f}, 1.0f), */
        /* Cube({-4.0f, -4.0f, -14.0f}, 1.0f), */
        /* Cube({0.0f, -4.0f, -18.0f}, 1.0f), */ 
        /* Cube({4.0f, -4.0f, -18.0f}, 1.0f), */
        /* Cube({-4.0f, -4.0f, -18.0f}, 1.0f), */

        /* Cube({0.0f, 4.0f, -10.0f}, 1.0f), */ 
        /* Cube({4.0f, 4.0f, -10.0f}, 1.0f), */
        /* Cube({-4.0f, 4.0f, -10.0f}, 1.0f), */
        /* Cube({0.0f, 4.0f, -14.0f}, 1.0f), */ 
        /* Cube({4.0f, 4.0f, -14.0f}, 1.0f), */
        /* Cube({-4.0f, 4.0f, -14.0f}, 1.0f), */
        /* Cube({0.0f, 4.0f, -18.0f}, 1.0f), */ 
        /* Cube({4.0f, 4.0f, -18.0f}, 1.0f), */
        /* Cube({-4.0f, 4.0f, -18.0f}, 1.0f), */

        Cube({10.0f, 0.0f, -10.0f}, 1.0f), 
        // debug
        Cube({0.0f, 0.0f, 10.0f}, 1.0f), 
        Cube({0.0f, 10.0f, 0.0f}, 1.0f), 
        Cube({0.0f, -10.0f, 0.0f}, 1.0f), 
        Cube({10.0f, 0.0f, 0.0f}, 1.0f), 
        Cube({-10.0f, 0.0f, 0.0f}, 1.0f), 
        // debug end
    };
}

std::vector<Triangle*> init_triangles() {
    return std::vector<Triangle*> {
        new Triangle({0.0f, 0.0f, -10.0f}, {10.0f, 10.0f, -10.0f}, {10.0f, 0.0f, -10.0f}, BLACK),
        new Triangle({-5.0f, -5.0f, -20.0f}, {10.0f, 10.0f, -20.0f}, {10.0f, 0.0f, -20.0f}),
    };
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Ustawienie multisamplingu na 4x
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(NULL));

    float side_size = 1.0f;
    /* int cubes_n = 9; */
    std::vector<Triangle*> triangles = init_triangles();

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
    Vector3 camera_pos = (Vector3){0.0f, 0.0f, 0.0f};
    Vector3 camera_target = (Vector3){0.0f, 0.0f, -1.0f};
    Vector3 camera_right = Vector3CrossProduct(camera_target, camera_up);
    Vcam camera = Vcam(camera_pos, camera_up, camera_target, project_mat);
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

    BSPTree bsp_tree = BSPTree(triangles);
    
    InitWindow(screenWidth, screenHeight, "Virtual camera");

    SetTargetFPS(60);
    DisableCursor();
    /* rlSetCullFace(RL_CULL_FACE_BACK); */
    /* rlEnableWireMode(); */
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        //----------------------------------------------------------------------------------
        
        Vector2 mouse_delta = GetMouseDelta();
        if(mouse_delta.x != 0 || mouse_delta.y != 0) {
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera.camera_pos, mouse_delta.x * mouse_sensitivity);
            Quaternion q_yaw_camera = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, -mouse_delta.x * mouse_sensitivity);

            Quaternion q_pitch_rot = QuaternionFromAxisAngle({-1.0, 0.0, 0.0}, mouse_delta.y * mouse_sensitivity);
            Quaternion q_pitch_camera = QuaternionFromAxisAngle(camera.camera_right, -mouse_delta.y * mouse_sensitivity);

            camera.camera_target = Vector3RotateByQuaternion(camera.camera_target, q_yaw_camera);
            camera.camera_right = Vector3RotateByQuaternion(camera.camera_right, q_yaw_camera);

            camera.camera_target = Vector3RotateByQuaternion(camera.camera_target, q_pitch_camera);
            camera.camera_up = Vector3RotateByQuaternion(camera.camera_up,  q_pitch_rot);

            for(auto& t : triangles) {
                t->rotate(q_yaw_rot);
                t->rotate(q_pitch_rot);
            }
        }

        // debug 

        if(IsKeyDown(KEY_H)) {
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera.camera_up, -7.0f * mouse_sensitivity);
            Quaternion q_yaw_camera = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, 7.0 * mouse_sensitivity);
            camera.camera_target = Vector3RotateByQuaternion(camera.camera_target, q_yaw_camera);
            camera.camera_right = Vector3RotateByQuaternion(camera.camera_right, q_yaw_camera);

            for(auto& t : triangles)
                t->rotate(q_yaw_rot);
        }

        if(IsKeyDown(KEY_L)) {
            Quaternion q_yaw_rot = QuaternionFromAxisAngle(camera.camera_up, 7.0f * mouse_sensitivity);
            Quaternion q_yaw_camera = QuaternionFromAxisAngle({0.0f, 1.0f, 0.0f}, -7.0 * mouse_sensitivity);
            camera.camera_target = Vector3RotateByQuaternion(camera.camera_target, q_yaw_camera);
            camera.camera_right = Vector3RotateByQuaternion(camera.camera_right, q_yaw_camera);

            for(auto& t : triangles)
                t->rotate(q_yaw_rot);
        }

        if(IsKeyDown(KEY_J)) {
            Quaternion q_pitch_rot = QuaternionFromAxisAngle({-1.0f, 0.0f, 0.0f}, 7.0f * mouse_sensitivity);
            Quaternion q_pitch_camera = QuaternionFromAxisAngle(camera.camera_right, -7.0f * mouse_sensitivity);
            camera.camera_target = Vector3RotateByQuaternion(camera.camera_target, q_pitch_camera);
            camera.camera_up = Vector3RotateByQuaternion(camera.camera_up,  q_pitch_rot);

            for(auto& t : triangles)
                t->rotate(q_pitch_rot);
        }

        if(IsKeyDown(KEY_K)) {
            Quaternion q_pitch_rot = QuaternionFromAxisAngle({-1.0f, 0.0f, 0.0f}, -7.0f * mouse_sensitivity);
            Quaternion q_pitch_camera = QuaternionFromAxisAngle(camera.camera_right, 7.0f * mouse_sensitivity);
            camera.camera_target = Vector3RotateByQuaternion(camera.camera_target, q_pitch_camera);
            camera.camera_up = Vector3RotateByQuaternion(camera.camera_up, q_pitch_rot);

            for(auto& t : triangles)
                t->rotate(q_pitch_rot);
        }

        // debug end

        if(IsKeyDown(KEY_Q)) {
            Quaternion q_roll_rot = QuaternionFromAxisAngle({0.0, 0.0, 1.0}, 10.0f * mouse_sensitivity);
            for(auto& t : triangles) {
                t->rotate(q_roll_rot);
            }
        }
        if(IsKeyDown(KEY_E)) {
            Quaternion q_roll_rot = QuaternionFromAxisAngle({0.0, 0.0, -1.0}, 10.0f * mouse_sensitivity);    
            for(auto& t : triangles) {
                t->rotate(q_roll_rot);
            }
        }

        if(IsKeyDown(KEY_W)) {
            camera.camera_pos = Vector3Add(camera.camera_pos, Vector3Scale(Vector3Normalize(camera.camera_target), 0.1));
            for(auto& t : triangles)
                t->multiply_by_matrix(move_forward);
        }
        if(IsKeyDown(KEY_S)) {
            camera.camera_pos = Vector3Subtract(camera.camera_pos, Vector3Scale(Vector3Normalize(camera.camera_target), 0.1));
            for(auto& t : triangles)
                t->multiply_by_matrix(move_backward);
        }
        if(IsKeyDown(KEY_A)) {
            camera.camera_pos = Vector3Subtract(camera.camera_pos, Vector3Scale(Vector3Normalize(camera.camera_right), 0.1));
            for(auto& t : triangles)
                t->multiply_by_matrix(move_left);
        }
        if(IsKeyDown(KEY_D)) {
            camera.camera_pos = Vector3Add(camera.camera_pos, Vector3Scale(Vector3Normalize(camera.camera_right), 0.1));
            for(auto& t : triangles)
                t->multiply_by_matrix(move_right);
        }
        if(IsKeyDown(KEY_SPACE)) {
            camera.camera_pos = Vector3Add(camera.camera_pos, Vector3Scale((Vector3){0.0f, 1.0f, 0.0f}, 0.1f));
            for(auto& t : triangles)
                t->multiply_by_matrix(move_up);
        }
        if(IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.camera_pos = Vector3Add(camera.camera_pos, Vector3Scale((Vector3){0.0f, -1.0f, 0.0f}, 0.1f));
            for(auto& t : triangles)
                t->multiply_by_matrix(move_down);
        }
        if(IsKeyDown(KEY_B)) {
            printf("break\n");
            printf("break\n");
            printf("break\n");
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
        
        bsp_tree.draw(camera);

        DrawCircle(screenWidth/2, screenHeight/2, 7.5f, RED);


        DrawText(TextFormat("Move: WSAD, SPACE, LCTRL"), 20, 20, 20, BLACK);
        DrawText(TextFormat("Rotate: Mouse, Q, E"), 20, 40, 20, BLACK);
        DrawText(TextFormat("Zoom: +, -"), 20, 60, 20, BLACK);
        DrawText(TextFormat("Zoom x%.2f", default_fovy/fovy), 20, 80, 20, BLACK);

        DrawText(TextFormat("Camera x target %f", camera.camera_target.x), 20, 120, 20, BLACK);
        DrawText(TextFormat("Camera y target %f", camera.camera_target.y), 20, 140, 20, BLACK);
        DrawText(TextFormat("Camera z target %f", camera.camera_target.z), 20, 160, 20, BLACK);

        DrawText(TextFormat("Camera x up %f", camera.camera_up.x), 20, 200, 20, BLACK);
        DrawText(TextFormat("Camera y up %f", camera.camera_up.y), 20, 220, 20, BLACK);
        DrawText(TextFormat("Camera z up %f", camera.camera_up.z), 20, 240, 20, BLACK);

        DrawText(TextFormat("Camera x right %f", camera.camera_right.x), 20, 280, 20, BLACK);
        DrawText(TextFormat("Camera y right %f", camera.camera_right.y), 20, 300, 20, BLACK);
        DrawText(TextFormat("Camera z right %f", camera.camera_right.z), 20, 320, 20, BLACK);

        DrawText(TextFormat("Camera x pos %f", camera.camera_pos.x), 20, 360, 20, BLACK);
        DrawText(TextFormat("Camera y pos %f", camera.camera_pos.y), 20, 380, 20, BLACK);
        DrawText(TextFormat("Camera z pos %f", camera.camera_pos.z), 20, 400, 20, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

