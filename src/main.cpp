#include <iostream>
#include "../include/Ray.h"
#include "../include/Vec3.h"
#include <fstream> 
#include "../include/Sphere.h" 
#include "../include/Material.h"
#include <vector>
#include "../include/Object.h"
#include "../include/Plane.h"
#include "../include/Logic.h"
#include "../include/BVHNode.h"

int main(int argc, char** argv) {
	const int height = 100;
    const int width = 200;
    const int pixle_sample=10000; 
    double aspect_ratio = double(width) / height;
	std::ofstream out("output/image.ppm", std::ios::trunc);
	out << "P3\n" << width << " " << height << "\n255\n";
	std::vector<Object*> objects;
	
	Object* sphere1 = new Sphere(1.0, Vec3(0, 0, -2), new LambertianMaterial(Vec3(0.8, 0.2, 0.2),0.8));
	Object* ground = new Plane(Vec3(0, 1, 0), -1.0, new LambertianMaterial(Vec3(0.3, 0.3, 0.3), 0.8));

    objects.push_back(ground);
	objects.push_back(sphere1);
	objects.push_back(
    new Sphere( 0.5,Vec3(-2, 0, -2),
               new MetalMaterial(Vec3(0.8, 0.8, 0.8), 0.0)));

	objects.push_back(
    new Sphere(0.4,Vec3(0.8, 0, -2), 
               new MetalMaterial(Vec3(1.0, 1.0, 0.2), 0.2)));

	BVHNode* bvh_root = build_bvh(objects, 0, objects.size()-1, 0);

	for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; ++i) {
        	Vec3 color(0,0,0);
        	for(int s=0;s<pixle_sample;s++){
        		double dx=((double)std::rand()/RAND_MAX)-0.5;
                double dy=((double)std::rand()/RAND_MAX)-0.5;
        		double u = (double(i)+dx) / width;
           		double v = (double(j)+dy) / height;
            Vec3 dir(
                2.0 * aspect_ratio * u - aspect_ratio,
                2.0 * v - 1.0,
                -1.02
            );
            dir = dir.normalize();
			Ray ray(Vec3(0, 0, 0), dir);
			
			Vec3 ray_color = compute_color(0, ray, bvh_root);
			color = color + ray_color;
        	}
            color = color / pixle_sample;

			double gamma = 2.2;
			color.x = std::pow(color.x, 1.0/gamma);
			color.y = std::pow(color.y, 1.0/gamma);
			color.z = std::pow(color.z, 1.0/gamma);
			color = color.clamp();

			int r = int(255 * color.x);
			int g = int(255 * color.y);
			int b = int(255 * color.z);
			out << r << " " << g << " " << b << "\n";
        }
    }
	
	for (int k = 0; k < objects.size(); ++k) {
		delete objects[k]->mat;
		delete objects[k];
	}

	return 0;
}