#pragma once
#include <iostream>
#include "Vec3.h"
#include "Ray.h"
#include "Sphere.h"
#include "Material.h"
#include "Object.h"
#include <fstream>
#include <cmath>
#include <vector>
#include "Plane.h"
#include <cstdlib>
#include <algorithm>
#include "BVHNode.h"
extern int MAX_DEPTH;
extern bool USE_RUSSIAN_ROULETTE;
Vec3 compute_color(int depth,const Ray &ray,BVHNode *bvh_root);
BVHNode *build_bvh(std::vector<Object*> &objects,int start,int end,int depth=0);
extern Object* bvh_hit_object;
double bvh_intersect(BVHNode *node,const Ray &ray);
Vec3 random_hemisphere_direction(const Vec3 &normal);
