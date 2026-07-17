#include "../include/BVHNode.h"
#include "../include/Logic.h"
#include "../include/Material.h"
#include "../include/Mesh.h"
#include "../include/Object.h"
#include "../include/Plane.h"
#include "../include/Ray.h"
#include "../include/Sphere.h"
#include "../include/Texture.h"
#include "../include/Vec3.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

int md;
bool urr;
const int SSP = 200;
int main(int argc, char **argv) {
  md = MAX_DEPTH;
  urr = USE_RUSSIAN_ROULETTE;
  const int height = 200;
  const int width = 400;
  double samples_per_pixel = SSP;
  double aspect_ratio = double(width) / height;
  std::ofstream out("output/image.ppm", std::ios::trunc);
  out << "P3\n" << width << " " << height << "\n255\n";
  std::vector<Object *> objects;
  OBJData data = OBJData::load("quad.obj");


  Mesh *mesh = new Mesh(
      data,
      new LambertianMaterial(
    new ImageTexture("assets/textures/uv_direction.ppm"), 0.9));
  objects.push_back(mesh);
  // 1. dielectric (glass)
  objects.push_back(
      new Sphere(Vec3(-2.4, -0.45, -2.3), 0.55, new DielectricMaterial(1.5)));

  // 2. golden-metal
  objects.push_back(new Sphere(
      Vec3(-0.85, -0.5, -2.2), 0.5,
      new MetalMaterial(new SolidColorTexture(Vec3(1.0, 0.8, 0.2)), 0.1)));

  // 3. red-lambertian
  objects.push_back(new Sphere(
      Vec3(0.25, -0.5, -2.0), 0.5,
      new LambertianMaterial(new SolidColorTexture(Vec3(1.0, 0.2, 0.2)), 0.8)));

  // plane (checkerboard)
  objects.push_back(new Plane(
      Vec3(0, 1, 0), -1.0,
      new LambertianMaterial(
          new CheckerTexture(Vec3(0.8, 0.8, 0.8), Vec3(0.2, 0.2, 0.2), 4.0),
          0.9)));
  validate_scene(objects);
  std::chrono::high_resolution_clock::time_point start_bvh =
      std::chrono::high_resolution_clock::now();
  BVHNode *bvh_root = build_bvh(objects, 0, objects.size() - 1);
  std::chrono::high_resolution_clock::time_point end_bvh =
      std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> bvh_time = end_bvh - start_bvh;
  std::cout << "BVH build time: " << bvh_time.count() << " seconds\n";

  int total_pixels = width * height;
  int processed_pixels = 0;
  int last_percent = 0;

  std::chrono::high_resolution_clock::time_point start_render =
      std::chrono::high_resolution_clock::now();

  for (int j = height - 1; j >= 0; j--) {
    for (int i = 0; i < width; ++i) {
      Vec3 color(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; s++) {
        double dx = ((double)std::rand() / RAND_MAX) - 0.5;
        double dy = ((double)std::rand() / RAND_MAX) - 0.5;
        double u = (double(i) + dx) / width;
        double v = (double(j) + dy) / height;
        Vec3 dir(2.0 * aspect_ratio * u - aspect_ratio, 2.0 * v - 1.0, -1.02);
        dir = dir.normalize();
        Ray ray(Vec3(0, 0, 0), dir);

        Vec3 ray_color = compute_color(0, ray, bvh_root);
        color = color + ray_color;
      }
      color = color / (double)samples_per_pixel;

      double gamma = 2.2;
      color.x = std::pow(color.x, 1.0 / gamma);
      color.y = std::pow(color.y, 1.0 / gamma);
      color.z = std::pow(color.z, 1.0 / gamma);
      color = color.clamp();

      int r = int(255 * color.x);
      int g = int(255 * color.y);
      int b = int(255 * color.z);
      out << r << " " << g << " " << b << "\n";

      processed_pixels++;
      int current_percent = (processed_pixels * 100) / total_pixels;
      if (current_percent >= last_percent + 10) {
        std::cout << "Render progress: " << current_percent << "%\n";
        last_percent = current_percent;
      }
    }
  }
  std::chrono::high_resolution_clock::time_point end_render =
      std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> render_time = end_render - start_render;

  long long total_rays = (long long)width * height * samples_per_pixel;
  double rays_per_second = total_rays / render_time.count();

  std::cout << "Render time: " << render_time.count() << " seconds\n";
  std::cout << "Total rays: " << total_rays << "\n";
  std::cout << "Rays per second: " << rays_per_second << " rays/s\n";

  for (int k = 0; k < objects.size(); ++k) {
    delete objects[k]->mat;
    delete objects[k];
  }

  delete bvh_root;

  out.close();
  return 0;
}