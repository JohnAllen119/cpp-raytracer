#pragma once

#include "Vec3.h"
#include <vector>
#include <string>
#include <array>
class OBJData {
public:
    std::vector<Vec3> vertices;
    std::vector<std::array<int, 3>> faces;

    static OBJData load(const std::string& filename);
};