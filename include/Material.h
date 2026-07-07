#pragma once
#include "Vec3.h"
#include "Ray.h"
#include "Object.h"
#include "Texture.h"

class Material {
public:
    virtual bool scatter(
        const Ray &rayIn,
        const Vec3 &normal,
        const Vec3 hitpoint,
        double u,
        double v,
        Ray &scatter,
        Vec3 &attenuation,
        Object* obj
    ) const = 0;

    virtual ~Material() = default;
};

class LambertianMaterial : public Material {
public:
    double albedo;
    Texture* texture;

    LambertianMaterial(Texture* c, double a) : texture(c), albedo(a) {}

    bool scatter(
        const Ray &rayIn,
        const Vec3 &normal,
        const Vec3 hitpoint,
        double u,
        double v,
        Ray &scatter,
        Vec3 &attenuation,
        Object* obj
    ) const override;
};

class MetalMaterial : public Material {
public:
    Texture* texture;
    double roughness;

    MetalMaterial(Texture* texture, double r) : texture(texture), roughness(r) {}

    bool scatter(
        const Ray &rayIn,
        const Vec3 &normal,
        const Vec3 hitpoint,
        double u,
        double v,
        Ray &scatter,
        Vec3 &attenuation,
        Object* obj
    ) const override;
};

class DielectricMaterial : public Material {
public:
    double ir;

    DielectricMaterial(double a) : ir(a) {}

    bool scatter(
        const Ray &rayIn,
        const Vec3 &normal,
        const Vec3 hitpoint,
        double u,
        double v,
        Ray &scatter,
        Vec3 &attenuation,
        Object* obj
    ) const override;
};

