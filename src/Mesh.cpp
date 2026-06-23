#include"../include/Mesh.h"


Mesh::Mesh(const OBJData& data) : Object(nullptr) {
    for (const auto& face : data.faces) {
        int i = face[0];
        int j = face[1];
        int k = face[2];
        triangles.emplace_back(data.vertices[i], data.vertices[j], data.vertices[k], nullptr);
    }
}

Mesh::Mesh(const OBJData& data, Material* m) : Object(m) {
    for (const auto& face : data.faces) {
        int i = face[0];
        int j = face[1];
        int k = face[2];
        triangles.emplace_back(data.vertices[i], data.vertices[j], data.vertices[k], m);
    }
}

Vec3 Mesh::getnormal(const Vec3& point) const {
    if (triangles.empty()) return Vec3(0, 1, 0);
    return triangles[0].getnormal(point);
}

AABB Mesh::getbounding() const {
    if (triangles.empty()) return AABB(Vec3(0, 0, 0), Vec3(0, 0, 0));
    
    Vec3 min = triangles[0].getbounding().min_bound;
    Vec3 max = triangles[0].getbounding().max_bound;
    
    for (const auto& tri : triangles) {
        AABB box = tri.getbounding();
        min = Vec3(std::min(min.x, box.min_bound.x), std::min(min.y, box.min_bound.y), std::min(min.z, box.min_bound.z));
        max = Vec3(std::max(max.x, box.max_bound.x), std::max(max.y, box.max_bound.y), std::max(max.z, box.max_bound.z));
    }
    
    return AABB(min, max);
}

void Mesh::getuv(double& u, double& v, const Vec3& hitPoint) const {
    u = 0.0;
    v = 0.0;
}