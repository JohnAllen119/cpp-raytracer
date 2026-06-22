#pragma once

#include "Ray.h"
#include "Vec3.h"
#include "AABB.h"
#include "Vec2.h"
class Material;
struct hit_record;
class Object{
	public:
	Material *mat;
	Object(Material *m):mat(m){}
	virtual ~Object(){}
	virtual Vec3 getnormal(const Vec3 &p)const=0;
	virtual bool is_hit(const Ray &ray,double t_min,double t_max,hit_record &rec)const=0;
	virtual AABB getbounding()const=0;
	virtual void getuv(double &u,double &v,const Vec3 &hitPoint)const=0;
};
struct hit_record{
    Vec3 p;
    Vec3 normal;
    double t;
    Material *mat;
    Object *obj;
    double u,v;
    hit_record():t(1e10),mat(NULL),obj(NULL){}

};
