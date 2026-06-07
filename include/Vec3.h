#pragma once
#include<cmath>
#include<algorithm>
struct Vec3{
	double x,y,z;
	Vec3():x(0.0),y(0.0),z(0.0){}
	Vec3(double _x,double _y,double _z):x(_x),y(_y),z(_z){}
	
	Vec3 operator+(const Vec3 &b)const{
		return Vec3((x+b.x),(y+b.y),(z+b.z));
	}
	Vec3 operator*(double t) const{
		return Vec3((x*t),(y*t),(z*t));
	}
	Vec3 operator/(double t) const{
		return Vec3((x/t),(y/t),(z/t));
	}
	double length() const{
		return std::sqrt(x*x+y*y+z*z);
	}
	Vec3 operator-(const Vec3 &b) const{
		return Vec3((x-b.x),(y-b.y),(z-b.z));
	}
	Vec3 cross(const Vec3 &b)const{
		return Vec3(
            y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x
        );
	}
	double dot(const Vec3 &b) const{
		return x*b.x+y*b.y+z*b.z;
	}
	
	Vec3 normalize() const{
		return *this/length();
	}
	Vec3 operator-()const{
		return Vec3(-x,-y,-z);
	}
	double operator[](int i)const{
		if(i==0) return x;
		if(i==1) return y;
		return z;
	}
	Vec3 clamp()const{
		return Vec3(
		std::max(0.0,std::min(1.0,x)),
		std::max(0.0,std::min(1.0,y)),
		std::max(0.0,std::min(1.0,z))
		);
	}
};
