#include "../include/OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
OBJData OBJData::load(const std::string& filename) {
    OBJData data;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open OBJ file");
    }

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            double x, y, z;
            ss >> x >> y >> z;
            data.vertices.emplace_back(x, y, z);
        }

        else if (type == "f") {
            int i, j, k;
            ss >> i >> j >> k;

            
            data.faces.push_back({{i - 1, j - 1, k - 1}});
        }
    }


    return data;
}