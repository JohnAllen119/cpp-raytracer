#pragma once
#include"Vec3.h"
#include"Ray.h"
#include "Material.h"
#include "Object.h"
#include "AABB.h"
struct Sphere :public Object{
	Vec3 center;
	double radius;
	
	Sphere(Vec3 c,double r,Material *ma):center(c),radius(r),Object(ma){}
	
	double is_hit(const Ray &ray)const override{
		Vec3 oc=ray.origin-center;
		double a=ray.direction.dot(ray.direction);
		double half_b=oc.dot(ray.direction);
		double c=(oc.dot(oc)-radius*radius);
		double discriminant=half_b*half_b-a*c;
		if(discriminant<0){
			return -1;
		}
		
		double tmp=std::sqrt(discriminant);
		double t=(-half_b-tmp)/a;
		if(t>0) return t;
		t=(-half_b+tmp)/a;
		if(t>0) return t;
		return -1;
	}
	
	Vec3 getnormal(const Vec3 &p)const override{
		 Vec3 tmp=p-center;
		 return tmp.normalize();
	}
	AABB getbounding() const override{
		return AABB(center-Vec3(radius,radius,radius),center+Vec3(radius,radius,radius));
	}
};

