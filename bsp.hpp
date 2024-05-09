#ifndef BSP_HPP
#define BSP_HPP

#include "include/raylib.h"
#include <vector>
#include "util.hpp"

class BSPNode {
public:
    Triangle* triangle;
    Vector3 triangle_normal;
    Vector3 point_on_triangle;
    BSPNode* behind;
    BSPNode* front;

    BSPNode(Triangle* triangle);

    bool camera_in_front(const Vcam& camera) const;
};

class BSPTree {
private:
    BSPNode* root;

    int line_intersection_with_plane(Vector3 p1, Vector3 p2, Vector4 plane, Vector3* out_point) const;

    // split triangle using plane
    void split(Triangle* t, Vector4 plane, std::vector<Triangle*>& triangles);

    std::vector<Triangle*> find_front(Triangle* triangle, std::vector<Triangle*>& triangles) const;

    std::vector<Triangle*> find_behind(Triangle* triangle, std::vector<Triangle*>& triangles) const;

    void make_bsp_tree(BSPNode* node, std::vector<Triangle*>& triangles);

    void draw(const Vcam& camera, BSPNode* node) const;

    void restore_triangles(std::vector<Triangle*>& triangles);
    
    void restore_triangles(BSPNode* node, std::vector<Triangle*>& triangles);

public:
    BSPTree(std::vector<Triangle*>& triangles);

    void draw(Vcam camera) const;
};

#endif
