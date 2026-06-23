#pragma once
#include "Object.h"

class Triangle : public Object {
public:
  Vec3 v0, v1, v2;
  Vec2 uv0, uv1, uv2;
  Triangle(Vec3 v0, Vec3 v1, Vec3 v2, Material *mat)
      : v0(v0), v1(v1), v2(v2), Object(mat), uv0(0, 0), uv1(0, 0), uv2(0, 0) {}
  Triangle(Vec3 v0, Vec3 v1, Vec3 v2)
      : v0(v0), v1(v1), v2(v2), Object(nullptr), uv0(0, 0), uv1(0, 0), uv2(0, 0) {}
  bool is_hit(const Ray &ray, double t_min, double t_max,
              hit_record &rec) const override {
    Vec3 E1 = v1 - v0;
    Vec3 E2 = v2 - v0;
    Vec3 P = ray.direction.cross(E2);
    double det = E1.dot(P);

    if (det > -1e-6 && det < 1e-6)
      return false;
    double inv_det = 1.0 / det;
    Vec3 T = ray.origin - v0;
    double u = T.dot(P) * inv_det;
    if (u < 0.0 || u > 1.0)
      return false;
    Vec3 Q = T.cross(E1);
    double v = ray.direction.dot(Q) * inv_det;
    if (v < 0.0 || u + v > 1.0)
      return false;

    double t = E2.dot(Q) * inv_det;
    rec.t = t;
    rec.p = ray.at(t);
    double tex_u, tex_v;
    getuv(tex_u, tex_v, rec.p);
    rec.normal = getnormal(rec.p);
    rec.mat = mat;
    rec.obj = const_cast<Triangle *>(this);

    rec.u = tex_u;
    rec.v = tex_u;
    return true;
  }
  Vec3 getnormal(const Vec3 &Point)const override{
    Vec3 E1=v1-v0;
    Vec3 E2=v2-v0;
    return E1.cross(E2).normalize();
  }
  AABB getbounding() const override{
        Vec3 min(
        std::min({v0.x, v1.x, v2.x}),
        std::min({v0.y, v1.y, v2.y}),
        std::min({v0.z, v1.z, v2.z})
    );
    Vec3 max(
        std::max({v0.x, v1.x, v2.x}),
        std::max({v0.y, v1.y, v2.y}),
        std::max({v0.z, v1.z, v2.z})
    );
    return AABB(min, max);
    }
    void getuv(double &u, double &v, const Vec3 &hitPoint) const override{
        //hitpoint_weight
        Vec3 E1=v1-v0;
        Vec3 E2=v2-v0;
        Vec3 P=hitPoint-v0;
        double d00=E1.dot(E1);
        double d01=E1.dot(E2);
        double d11=E2.dot(E2);
        double d20=P.dot(E1);
        double d21=P.dot(E2);
        double denom=d00*d11-d01*d01;
        double beta=(d11*d20-d01*d21)/denom;
        double gamma=(d00*d21-d01*d20)/denom;
        double alpha=1.0-beta-gamma;

        u=uv0.x*alpha+uv1.x*beta+uv2.x*gamma;
        v=uv0.y*alpha+uv1.y*beta+uv2.y*gamma;
        
    }
};