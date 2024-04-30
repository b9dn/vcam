#include "include/raylib.h"
#include "include/raymath.h"
#include <algorithm>
#include "util.hpp"
#include "bsp.hpp"

BSPNode::BSPNode(Triangle* triangle) {
    this->triangle = triangle;
    this->point_on_triangle = triangle->verticies[0];
    this->behind = NULL;
    this->front = NULL;

    auto v1 = Vector3Subtract(triangle->verticies[0], triangle->verticies[1]);
    auto v2 = Vector3Subtract(triangle->verticies[0], triangle->verticies[2]);
    this->triangle_normal = Vector3CrossProduct(v1, v2);
}

bool BSPNode::camera_in_front(const Vcam& camera) {
    auto camera_vector = Vector3Subtract(camera.camera_pos, point_on_triangle);
    return Vector3DotProduct(camera_vector, triangle_normal) > 0 ? true : false;
}

int BSPTree::line_intersection_with_plane(Vector3 p1, Vector3 p2, Vector4 plane, Vector3* out_point) {
    float denominator = plane.x * (p2.x - p1.x) + 
                    plane.y * (p2.y - p1.y) + 
                    plane.z * (p2.z - p1.z);
    // plane parallel to line
    if(abs(denominator) < 1e-7f)
        return -1;

    float param = -((plane.x * p1.x + plane.y * p1.y + plane.z * p1.z + plane.w) / denominator);

    float intersection_x = p1.x + param * (p2.x - p1.x);
    float intersection_y = p1.y + param * (p2.y - p1.y);
    float intersection_z = p1.z + param * (p2.z - p1.z);

    *out_point = (Vector3){intersection_x, intersection_y, intersection_z};
    return 0;
}

// split triangle using plane
void BSPTree::split(Triangle* t, Vector4 plane, std::vector<Triangle*>& triangles) {
    triangles.erase(std::remove(triangles.begin(), triangles.end(), t), triangles.end());
    float sign_p1 = point_in_plane_equasion(t->verticies[0], plane);
    float sign_p2 = point_in_plane_equasion(t->verticies[1], plane);
    float sign_p3 = point_in_plane_equasion(t->verticies[2], plane);

    Vector3 one_side;
    Vector3 other_side1;
    Vector3 other_side2;
    Vector3 intersection1;
    Vector3 intersection2;

    // split on 2 triangles
    if(sign_p1 == 0) {
        if(line_intersection_with_plane(t->verticies[1], t->verticies[2], plane, &intersection1) < 0)
            return;
        Triangle* t1new = new Triangle(t->verticies[0], intersection1, t->verticies[1], t->color);
        Triangle* t2new = new Triangle(t->verticies[0], intersection1, t->verticies[2], t->color);
        triangles.push_back(t1new);
        triangles.push_back(t2new);
        return;
    }
    if(sign_p2 == 0) {
        if(line_intersection_with_plane(t->verticies[0], t->verticies[2], plane, &intersection1) < 0)
            return;
        Triangle* t1new = new Triangle(t->verticies[1], intersection1, t->verticies[0], t->color);
        Triangle* t2new = new Triangle(t->verticies[1], intersection1, t->verticies[2], t->color);
        triangles.push_back(t1new);
        triangles.push_back(t2new);
        return;
    }
    if(sign_p3 == 0) {
        if(line_intersection_with_plane(t->verticies[0], t->verticies[1], plane, &intersection1) < 0)
            return;
        Triangle* t1new = new Triangle(t->verticies[2], intersection1, t->verticies[0], t->color);
        Triangle* t2new = new Triangle(t->verticies[2], intersection1, t->verticies[1], t->color);
        triangles.push_back(t1new);
        triangles.push_back(t2new);
        return;
    }
    // split on 3 triangles
    else if((sign_p1 > 0 && sign_p2 > 0) || (sign_p1 < 0 && sign_p2 < 0)) {
        one_side = t->verticies[2];
        other_side1 = t->verticies[0];
        other_side2 = t->verticies[1];
    }
    else if((sign_p2 > 0 && sign_p3 > 0) || (sign_p2 < 0 && sign_p3 < 0)) {
        one_side = t->verticies[0];
        other_side1 = t->verticies[1];
        other_side2 = t->verticies[2];
    }
    else {
        one_side = t->verticies[1];
        other_side1 = t->verticies[0];
        other_side2 = t->verticies[2];
    }

    if(line_intersection_with_plane(one_side, other_side1, plane, &intersection1))
        return;
    if(line_intersection_with_plane(one_side, other_side2, plane, &intersection2))
        return;
    
    Triangle* t1new = new Triangle(one_side, intersection1, intersection2, t->color);
    Triangle* t2new = new Triangle(other_side1, intersection1, intersection2, t->color);
    Triangle* t3new = new Triangle(intersection2, other_side1, other_side2, t->color);

    triangles.push_back(t1new);
    triangles.push_back(t2new);
    triangles.push_back(t3new);
}

std::vector<Triangle*> BSPTree::find_front(Triangle* triangle, std::vector<Triangle*>& triangles) {
    std::vector<Triangle*> front_triangles;
    for(const auto& t : triangles) {
        auto test_result = triangle->plane_cross_triangle(t);
        if(test_result == 1)
            front_triangles.push_back(t);
    }

    return front_triangles;
}

std::vector<Triangle*> BSPTree::find_behind(Triangle* triangle, std::vector<Triangle*>& triangles) {
    std::vector<Triangle*> behind_triangles;
    for(const auto& t : triangles) {
        auto test_result = triangle->plane_cross_triangle(t);
        if(test_result == -1)
            behind_triangles.push_back(t);
    }

    return behind_triangles;
}

void BSPTree::make_bsp_tree(BSPNode* node, std::vector<Triangle*>& triangles) {
    if(triangles.size() <= 0 || node == NULL)
        return;
    
    // split triangles that cross node triangle
    int i = 0;
    auto current_node_triangle = node->triangle;
    while(i < triangles.size()) {
        auto test_result = current_node_triangle->plane_cross_triangle(triangles[i]);
        if(test_result == 0) {
            split(triangles[i], current_node_triangle->to_plane(), triangles);
            continue;
        }
        i++;
    }

    auto behind_triangles = find_behind(current_node_triangle, triangles);
    auto front_triangles = find_front(current_node_triangle, triangles);

    if(behind_triangles.size() > 0) {
        node->behind = new BSPNode(behind_triangles[0]);
        behind_triangles.erase(behind_triangles.begin());
    }
    if(front_triangles.size() > 0) {
        node->front = new BSPNode(front_triangles[0]);
        front_triangles.erase(front_triangles.begin());
    }

    make_bsp_tree(node->behind, behind_triangles);
    make_bsp_tree(node->front, front_triangles);
}

void BSPTree::draw(const Vcam& camera, BSPNode* node) {
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

void BSPTree::restore_triangles(std::vector<Triangle*>& triangles) {
    triangles.clear();
    restore_triangles(root, triangles);
}

void BSPTree::restore_triangles(BSPNode* node, std::vector<Triangle*>& triangles) {
    if(node == NULL)
        return;

    triangles.push_back(node->triangle);
    restore_triangles(node->behind, triangles);
    restore_triangles(node->front, triangles);
}

BSPTree::BSPTree(std::vector<Triangle*>& triangles) {
    if(triangles.size() > 0) {
        root = new BSPNode(triangles[0]);
        triangles.erase(triangles.begin());
    }
    make_bsp_tree(root, triangles);
    restore_triangles(triangles);
}

void BSPTree::draw(Vcam camera) {
    draw(camera, root);
}
