#include "../include/Mesh.h"

Mesh::Mesh(const OBJData &data) : Object(nullptr) {
  for (const auto &face : data.faces) {
    int i = face.indices[0].v_index;
    int j = face.indices[1].v_index;
    int k = face.indices[2].v_index;
    triangles.emplace_back(data.vertices[i], data.vertices[j], data.vertices[k],
                           nullptr);
  }
}

Mesh::Mesh(const OBJData &data, Material *m) : Object(m) {
  for (const auto &face : data.faces) {
    int i = face.indices[0].v_index;
    int j = face.indices[1].v_index;
    int k = face.indices[2].v_index;

    Vec3 p0 = data.vertices[i];
    Vec3 p1 = data.vertices[j];
    Vec3 p2 = data.vertices[k];

    int uv_i = face.indices[0].vt_index;
    int uv_j = face.indices[1].vt_index;
    int uv_k = face.indices[2].vt_index;

    bool has_uv = uv_i >= 0 && uv_j >= 0 && uv_k >= 0 &&
                  uv_i < data.texcoords.size() &&
                  uv_j < data.texcoords.size() && uv_k < data.texcoords.size();

    int n_i = face.indices[0].vn_index;
    int n_j = face.indices[1].vn_index;
    int n_k = face.indices[2].vn_index;
    bool has_normal = n_i >= 0 && n_j >= 0 && n_k >= 0 &&
                      n_i < data.normals.size() && n_j < data.normals.size() &&
                      n_k < data.normals.size();
    if (has_uv && has_normal) {
      Vec2 uv0 = data.texcoords[uv_i];
      Vec2 uv1 = data.texcoords[uv_j];
      Vec2 uv2 = data.texcoords[uv_k];

      Vec3 n0 = data.normals[n_i].normalize();
      Vec3 n1 = data.normals[n_j].normalize();
      Vec3 n2 = data.normals[n_k].normalize();
      triangles.emplace_back(p0, p1, p2, uv0, uv1, uv2, n0, n1, n2, m);
    } else if (has_uv) {
      Vec2 uv0 = data.texcoords[uv_i];
      Vec2 uv1 = data.texcoords[uv_j];
      Vec2 uv2 = data.texcoords[uv_k];
      triangles.emplace_back(p0, p1, p2, uv0, uv1, uv2, m);
    } else if (has_normal) {
      Vec3 n0 = data.normals[n_i].normalize();
      Vec3 n1 = data.normals[n_j].normalize();
      Vec3 n2 = data.normals[n_k].normalize();

         triangles.emplace_back(p0, p1, p2,n0, n1, n2, m);
    }else{
        triangles.emplace_back(p0, p1, p2, m);
    }
  }
}
Vec3 Mesh::getnormal(const Vec3 &point) const {
  if (triangles.empty())
    return Vec3(0, 1, 0);
  return triangles[0].getnormal(point);
}

AABB Mesh::getbounding() const {
  if (triangles.empty())
    return AABB(Vec3(0, 0, 0), Vec3(0, 0, 0));

  Vec3 min = triangles[0].getbounding().min_bound;
  Vec3 max = triangles[0].getbounding().max_bound;

  for (const auto &tri : triangles) {
    AABB box = tri.getbounding();
    min =
        Vec3(std::min(min.x, box.min_bound.x), std::min(min.y, box.min_bound.y),
             std::min(min.z, box.min_bound.z));
    max =
        Vec3(std::max(max.x, box.max_bound.x), std::max(max.y, box.max_bound.y),
             std::max(max.z, box.max_bound.z));
  }

  return AABB(min, max);
}

void Mesh::getuv(double &u, double &v, const Vec3 &hitPoint) const {
  u = 0.0;
  v = 0.0;
}