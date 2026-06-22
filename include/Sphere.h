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
	
	bool is_hit(const Ray &ray,double t_min,double t_max,hit_record &rec)const override{
		Vec3 oc=ray.origin-center;
		double a=ray.direction.dot(ray.direction);
		double half_b=oc.dot(ray.direction);
		double c=(oc.dot(oc)-radius*radius);
		double discriminant=half_b*half_b-a*c;
		if(discriminant<0){
			return false;
		}
		
		double tmp=std::sqrt(discriminant);
		double t=(-half_b-tmp)/a;
		if(t<t_min||t>t_max){
			t=(-half_b+tmp)/a;
			if(t<t_min||t>t_max){
				return false;
			}
		}
		rec.t=t;
		rec.obj=const_cast<Sphere*>(this);
		rec.p=ray.at(t);
		rec.normal=(rec.p-center).normalize();
		rec.mat=this->mat;
		double u,v;
		getuv(u,v,rec.p);
		rec.u=u;
		rec.v=v;
		return true;
	}
	
	void getuv(double &u,double &v,const Vec3 &hitPoint)const override{
        const double M_PI=acos(-1.0);
        Vec3 p=hitPoint-center;
        p.normalize();
        double theta=atan2(p.z,p.x);
        double phi=acos(p.y);
        u=(theta+M_PI)/(2*M_PI);
        v=phi/M_PI;
    }
	Vec3 getnormal(const Vec3 &p)const override{
		 Vec3 tmp=p-center;
		 return tmp.normalize();
	}
	AABB getbounding() const override{
		return AABB(center-Vec3(radius,radius,radius),center+Vec3(radius,radius,radius));
	}
};

