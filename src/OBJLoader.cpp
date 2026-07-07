#include "../include/OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
static OBJIndex parseOBJIndex(const std::string& token){
    OBJIndex index;
    std::stringstream ss(token);
    std::string v_str;
    std::string vt_str;
    std::string vn_str;

    std::getline(ss,v_str,'/');
    std::getline(ss,vt_str,'/');
    std::getline(ss,vn_str,'/');

    if(!v_str.empty()){
        index.v_index=std::stoi(v_str)-1;
    }
    if(!vt_str.empty()){
        index.vt_index=std::stoi(vt_str);
    }
    if(!vn_str.empty()){
        index.vn_index=std::stoi(vn_str)-1;
    }
    return index;
}
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

        if (type.empty() || type[0] == '#') {
            continue;
        }
        if (type == "v") {
            double x, y, z;
            ss >> x >> y >> z;
            data.vertices.emplace_back(x, y, z);
        }
        else if (type == "vt") {
            double u, v;
            ss >> u >> v;
            data.texcoords.emplace_back(u, v);
        }

        else if (type == "vn") {
            double x, y, z;
            ss >> x >> y >> z;
            data.normals.emplace_back(x, y, z);
        }

        else if (type == "f") {
            std::string a, b, c;
            ss >> a >> b >> c;

            if (a.empty() || b.empty() || c.empty()) {
                throw std::runtime_error("Invalid face format");
            }

            OBJFace face;
            face.indices[0] = parseOBJIndex(a);
            face.indices[1] = parseOBJIndex(b);
            face.indices[2] = parseOBJIndex(c);

            data.faces.push_back(face);
        }
    }


    return data;
}