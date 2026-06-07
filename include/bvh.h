#pragma once
#include "aabb.h"
#include "object.h"

struct BVHNode {
    AABB box;
    BVHNode* left;
    BVHNode* right;
    Object* obj;

    BVHNode() : left(nullptr), right(nullptr), obj(nullptr) {}
    ~BVHNode() {
        delete left;
        delete right;
    }
};
