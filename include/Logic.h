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
Vec3 compute_refract_ray(double n1,double n2,const Vec3 &incident_dir,const Vec3 &normal);
Vec3 compute_color(int depth,const Ray &ray,std::vector<Object*> &object,std::vector<Light> &lights,BVHNode *bvh_root);
BVHNode *build_bvh(std::vector<Object*> &objects,int start,int end,int depth=0);
extern Object* bvh_hit_object;
double bvh_intersect(BVHNode *node,const Ray &ray);

Vec3 random_hemisphere_direction(const Vec3 &normal);
double ao_compute(const Vec3 &p,const Vec3 &normal,BVHNode *bvh_root,int samples,double max_dist);