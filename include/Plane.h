#pragma once
#include "Vec3.h"

#include "Ray.h"
#include "Object.h"
struct Plane : public Object {
     Vec3 normal;
     double d;
     Plane(Vec3 n,double dist,Material *mat): normal(n.normalize()),d(-dist),Object(mat){}

     double is_hit(const Ray &ray) const override {
        double denom=normal.dot(ray.direction);
        if(std::fabs(denom)<1e-6) return -1;

        return -(normal.dot(ray.origin)+d)/denom;
     }
     Vec3 getnormal(const Vec3 &p)const override{
        return normal;
     }
     AABB getbounding()const override{
        return AABB(Vec3(-100.0, -1.01, -100.0), Vec3(100.0, -0.99, 100.0));
     }
};
