#pragma once
#include "Vec3.h" 
#include "Ray.h"
class Material{
	public:
	virtual bool scatter(const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation)const=0;
	virtual ~Material()=default;
};
class LambertianMaterial: public Material{
	public:
	double albedo;
	Vec3 color;
	LambertianMaterial(Vec3 c,double a):color(c),albedo(a){}
	bool scatter(const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation)const override;
};
class MetalMaterial: public Material{
	public:
	Vec3 color;
	double roughness;
	MetalMaterial(Vec3 c,double r) :color(c),roughness(r){}
	bool scatter(const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation)const override;
};
class DielectricMaterial:public Material{
	public:
	double ir;
	DielectricMaterial(double a):ir(a){}
	bool scatter(const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation)const override;
};
struct Light{
	Vec3 color;
	double intensity;
	Vec3 direction;
	Light(Vec3 col,Vec3 dir,double inten):color(col),direction(dir),intensity(inten){}
};
