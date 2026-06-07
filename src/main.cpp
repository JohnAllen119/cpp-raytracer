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

int main(int argc, char** argv) {
	const int height = 200;
    const int width = 400;
    const int pixle_sample=16; 
    double aspect_ratio = double(width) / height;
	std::ofstream out("output/image.ppm", std::ios::trunc);
	out << "P3\n" << width << " " << height << "\n255\n";
	std::vector<Object*> object;
	std::vector<Light> lights;

	Sphere sphere1(1.0, Vec3(0, 0, -2), new Material(Vec3(0.8, 0.2, 0.2),0.8,30.0,0.5,0.001,0.001));
	Sphere sphere2(0.6, Vec3(2.0,0,-3), new Material(Vec3(0.5, 0.5, 0.5),0.8,50.0,0.1,0.95,1.5));
	Plane* ground = new Plane(Vec3(0, 1, 0), -1.0, new Material(Vec3(0.3, 0.3, 0.3), 0.8, 20.0,0.01,0.001,0.001));

    object.push_back(ground);
	object.push_back(&sphere1);
	object.push_back(&sphere2);

    lights.push_back(Light(Vec3(0.8, 1, 1), Vec3(2, 1.5, 1), 1.0));
	lights.push_back(Light(Vec3(0.1, 0.5, 0.2), Vec3(2.0, 1.0, 1), 0.5));

	BVHNode *bvh_root=build_bvh(object,0,object.size()-1);

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
			color=color+compute_color(0,ray,object,lights,bvh_root);
        	}
            color=color/pixle_sample;

			double gamma=2.2;
			color.x=std::pow(color.x,1.0/gamma);
			color.y=std::pow(color.y,1.0/gamma);
			color.z=std::pow(color.z,1.0/gamma);
			color=color.clamp();

			int r=int(255*color.x);
			int g=int(255*color.y);
			int b=int(255*color.z);
			out << r << " " << g << " " << b << "\n";
        }
    }

	return 0;
}
