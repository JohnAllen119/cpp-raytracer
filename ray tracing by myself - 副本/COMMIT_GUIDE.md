# Git Commit History Recommendations

Here's a suggested commit sequence to build the project incrementally for your GitHub repository:

## Initial Setup
```bash
git init
git add .gitignore README.md
git commit -m "Initial project setup: README and gitignore"
```

## Core Math & Geometry
```bash
git add include/Vec3.h include/Ray.h
git commit -m "Add Vec3 and Ray classes"

git add include/Material.h
git commit -m "Add Material and Light structures"

git add include/Object.h include/Sphere.h include/Plane.h
git commit -m "Add Object base class, Sphere, and Plane"
```

## AABB & BVH
```bash
git add include/AABB.h include/BVHNode.h
git commit -m "Add AABB bounding box and BVH node"

git add include/Logic.h src/Logic.cpp
git commit -m "Implement BVH acceleration structure"

git add src/main.cpp
git commit -m "Add main entry point with scene setup"

git add Makefile
git commit -m "Add Makefile for build system"
```

## Advanced Features
```bash
git add src/Logic.cpp
git commit -m "Add reflection and refraction with Fresnel and Snell's law"

git add src/Logic.cpp
git commit -m "Add Ambient Occlusion hemisphere sampling"

git add src/main.cpp
git commit -m "Add supersampling anti-aliasing and gamma correction"

git add output/.gitkeep
git commit -m "Add output directory placeholder"
```
