#pragma once
#include "Vec3.h"
#include "Ray.h"
#include <algorithm>
#include <vector>
#include <cmath> 
struct AABB{
	Vec3 min;
	Vec3 max;
	AABB():min(Vec3(0,0,0)),max(Vec3(0,0,0)){}
	AABB(const Vec3 &min,const Vec3 &max):min(min),max(max){}
	//���ܺ���-�жϹ����Ƿ���AABB�ཻ 
	bool intersect(const Ray &ray)const{
		double t_min=-1e10,t_max=1e10;
		for(int i=0;i<3;i++){
			if(std::fabs(ray.direction[i])<1e-8){
				continue;
			}else{
				double inv_dir=1.0/ray.direction[i];
				double t0=(min[i]-ray.origin[i])*inv_dir;
				double t1=(max[i]-ray.origin[i])*inv_dir;
				if(inv_dir<0) std::swap(t0,t1);
				t_min=std::max(t_min,t0);
				t_max=std::min(t_max,t1);
				if(t_max<t_min) return false;
			}
			
		}
		return t_max;
	}
	//�ϲ�����AABB 
	static AABB merge (const AABB &a,const AABB &b) {
		return AABB(
		Vec3(std::min(a.min.x,b.min.x),
			std::min(a.min.y,b.min.y),
			std::min(a.min.z,b.min.z)),
		Vec3(std::max(a.max.x,b.max.x),
			 std::max(a.max.y,b.max.y),
			 std::max(a.max.z,b.max.z))
		);
	}
};
