#pragma once

#include "Vec3.h"
#include <vector>
#include <string>
#include <array>
#include "Vec2.h"
class OBJIndex{
    public:
    int v_index  = -1;
    int vt_index = -1;
    int vn_index = -1;
};
class OBJFace{
    public:
    std::array<OBJIndex,3>indices;
};
class OBJData {
public:
    std::vector<Vec3> vertices;
    std::vector<OBJFace> faces;
    std::vector<Vec2> texcoords;
    std::vector<Vec3> normals;

    static OBJData load(const std::string& filename);
};