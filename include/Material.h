#pragma once
#include "Vec3.h" 
#include "Ray.h"
struct Material{
	Vec3 color;
	double albedo;
	double specular;
	double reflectivity;
	double transparency;
	double refractivity;
	Material(Vec3 col,double albe,double spe,double re,double tr,double ref):color(col),albedo(albe),specular(spe),reflectivity(re),transparency(tr),refractivity(ref){}
	
};


struct Light{
	Vec3 color;
	double intensity;
	Vec3 direction;
	Light(Vec3 col,Vec3 dir,double inten):color(col),direction(dir),intensity(inten){}
};
