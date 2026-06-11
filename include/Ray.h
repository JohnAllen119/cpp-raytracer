#pragma once
#include"Vec3.h"
struct Ray{
	Vec3 origin,direction;
	Ray():origin(Vec3(0,0,0)),direction(Vec3(0,0,0)){}
	Ray(const Vec3 &o,const Vec3 &d):origin(o),direction(d){}
	Vec3 at(double d) const{
		return (origin+direction*d);
	}
	
};
