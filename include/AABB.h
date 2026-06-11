#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <algorithm>
#include <vector>
#include <cmath> 

struct AABB{
	Vec3 min_bound;
	Vec3 max_bound;
	AABB():min_bound(Vec3(0,0,0)),max_bound(Vec3(0,0,0)){}
	AABB(const Vec3 &min_val,const Vec3 &max_val):min_bound(min_val),max_bound(max_val){}
	
	bool intersect(const Ray &ray)const{
		double t_min=-1e18,t_max=1e18;
		for(int i=0;i<3;i++){
			if(std::fabs(ray.direction[i])<1e-8){
				if(ray.origin[i] < min_bound[i] || ray.origin[i] > max_bound[i]){
					return false;
				}
				continue;
			}else{
				double inv_dir=1.0/ray.direction[i];
				double t0=(min_bound[i]-ray.origin[i])*inv_dir;
				double t1=(max_bound[i]-ray.origin[i])*inv_dir;
				if(inv_dir<0) std::swap(t0,t1);
				t_min=std::max(t_min,t0);
				t_max=std::min(t_max,t1);
				if(t_max<t_min) return false;
			}
			
		}
		return t_max > 0;
	}
	
	static AABB merge (const AABB &a,const AABB &b) {
		return AABB(
		Vec3(std::min(a.min_bound.x,b.min_bound.x),
			std::min(a.min_bound.y,b.min_bound.y),
			std::min(a.min_bound.z,b.min_bound.z)),
		Vec3(std::max(a.max_bound.x,b.max_bound.x),
			 std::max(a.max_bound.y,b.max_bound.y),
			 std::max(a.max_bound.z,b.max_bound.z))
		);
	}
};
