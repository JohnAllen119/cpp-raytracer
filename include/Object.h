#pragma once
#include "Material.h"
#include "Ray.h"
#include "Vec3.h"
#include "AABB.h"
class Object{
	public:
	Material *mat;
	Object(Material *m):mat(m){}
	virtual ~Object(){}
	virtual Vec3 getnormal(const Vec3 &p)const=0;
	virtual double is_hit(const Ray &ray)const=0;
	virtual AABB getbounding()const=0;
};
