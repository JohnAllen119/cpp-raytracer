#pragma once
#include "Vec3.h" 
#include "Ray.h"
struct Material{
	Vec3 color;
	double albedo;
	
	Material(Vec3 col,double albe):color(col),albedo(albe){}
	
};


struct Light{
	Vec3 color;
	double intensity;
	Vec3 direction;
	Light(Vec3 col,Vec3 dir,double inten):color(col),direction(dir),intensity(inten){}
};
