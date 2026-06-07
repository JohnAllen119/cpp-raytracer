# Whitted-Style Ray Tracer

Educational CPU-based Whitted-style ray tracing renderer in C++.

## Overview

This is a ray tracing renderer implemented for learning purposes. It implements core ray tracing techniques including
spherical and planar intersections, BVH acceleration, diffuse/specular lighting, reflection/refraction,
ambient occlusion, and supersampling anti-aliasing.

## Features

### Core Rendering
- [x] Perspective camera ray generation
- [x] Whitted-style recursive ray tracing
- [x] Supersampling anti-aliasing (16x)
- [x] Gamma correction (2.2)
- [x] PPM image output

### Geometry & Intersection
- [x] Sphere intersection (quadratic solver)
- [x] Plane intersection (infinite)
- [x] AABB bounding box (3-axis slab method)
- [x] BVH (Bounding Volume Hierarchy) acceleration structure

### Lighting & Materials
- [x] Lambert diffuse reflection
- [x] Blinn-Phong specular highlight
- [x] Shadow ray tracing
- [x] Perfect mirror reflection (recursive, 5 depth)
- [x] Refraction/Transparency (Fresnel + Snell's law)
- [x] Ambient Occlusion (hemisphere sampling, BVH accelerated)
- [x] Multiple light sources (spherical lights)

## Technical Implementation

### BVH Acceleration Structure
- **Building**: Recursively divides objects by alternating axes. Sorts objects by bounding box min on the current axis, splits in the middle, and merges children's AABBs.
- **Intersection**: Traverses BVH tree, testing bounding boxes first to prune entire branches quickly. Returns nearest hit.
- **Complexity**: O(log n) intersection vs O(n) brute-force.

### Ambient Occlusion
- **Sampling**: Generates random directions on hemisphere around normal using cosine-weighted sampling.
- **Testing**: Casts occlusion rays, checks for nearby intersections within max distance (2 units).
- **AO Factor**: 1 - (occlusion ratio * 0.8). Darkens creases and corners.

### Reflection & Refraction
- **Reflection**: Reflects incoming direction across normal using dot product.
- **Refraction**: Uses Snell's law to compute transmitted direction. Handles total internal reflection.

### Material System
Each object has a material with:
- `color`: Base albedo RGB
- `albedo`: Diffuse reflectivity
- `specular`: Phong exponent for shininess
- `reflectivity`: Mirror reflection blend
- `transparency`: Refractive transmission blend
- `refractivity`: Index of refraction (e.g., 1.5 for glass)

## Current Limitations
- Single-threaded rendering only
- Limited geometry (only spheres and infinite planes)
- No texture mapping
- Lights are point sources with hard shadows
- No depth of field, motion blur, or caustics
- BVH uses simple median split (not SAH heuristic)

## Future Work
- [ ] Multithreading (OpenMP or task-based parallelism)
- [ ] Triangle meshes (OBJ loading)
- [ ] Importance sampling for AO
- [ ] Texture mapping support
- [ ] Soft shadows (area lights)
- [ ] Depth of field and motion blur
- [ ] HDR environment maps
- [ ] SAH (Surface Area Heuristic) for better BVH

## Project Structure
```
raytracer/
├── include/
│   ├── AABB.h          # AABB bounding box
│   ├── BVHNode.h       # BVH node structure
│   ├── Logic.h         # Rendering functions declaration
│   ├── Material.h      # Material and Light structures
│   ├── Object.h        # Abstract Object base class
│   ├── Plane.h         # Plane geometry
│   ├── Ray.h           # Ray structure
│   ├── Sphere.h        # Sphere geometry
│   └── Vec3.h          # 3D vector math
├── src/
│   ├── Logic.cpp       # Rendering functions implementation
│   └── main.cpp        # Scene setup & entry point
├── output/             # Rendered images (gitignored)
├── Makefile
├── .gitignore
└── README.md
```

## Building & Running
```bash
make
./raytracer
# Outputs to output/image.ppm
# View with a PPM viewer or convert to PNG using ImageMagick:
# convert output/image.ppm output/image.png
```

## License
This project is created for educational purposes. Feel free to use it for learning.
