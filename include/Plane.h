#pragma once
#include "Vec3.h"

#include "Object.h"
#include "Ray.h"

struct Plane : public Object {
  Vec3 normal;
  double d;
  Plane(Vec3 n, double dist, Material *mat)
      : normal(n.normalize()), d(-dist), Object(mat) {}

  bool is_hit(const Ray &ray, double t_min, double t_max,
              hit_record &rec) const override {
    double denom = normal.dot(ray.direction);
    if (std::fabs(denom) < 1e-6)
      return false;

    double t = -(normal.dot(ray.origin) + d) / denom;
    if (t < t_min || t > t_max)
      return false;

    rec.t = t;
    rec.p = ray.at(t);
    rec.normal = normal;
    rec.mat = mat;
    rec.obj = const_cast<Plane *>(this);
    double u, v;
    getuv(u, v, rec.p);
    rec.u = u;
    rec.v = v;
    return true;
  }
  Vec3 getnormal(const Vec3 &p) const override { return normal; }
  AABB getbounding() const override {
    return AABB(Vec3(-100.0, -1.01, -100.0), Vec3(100.0, -0.99, 100.0));
  }

  void getuv(double &u, double &v, const Vec3 &hitPoint) const override {
    u = hitPoint.x * 0.2;
    v = hitPoint.z * 0.2;
  }
};