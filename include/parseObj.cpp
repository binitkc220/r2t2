#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "vec3.h"
std::vector<vec3> vertices;
std::vector<std::vector<int>> triangle_face;
std::vector<std::vector<int>> quad_face;

void readObjFile(std::string s) {
    float x,y,z;
    std::ifstream file(s);
    std::string line;

    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream ss(line.substr(2));
            ss >> x >> y >> z;
            vec3 v(x,y,z);
            vertices.push_back(v);
        } else if (line.substr(0, 2) == "f ") {
            std::istringstream ss(line.substr(2));
            std::vector<int> face;
            std::string token;
            while (ss >> token) {
                std::istringstream ts(token);
                int f;
                ts >> f;
                face.push_back(f);
            }
            if (face.size() == 3) {
                triangle_face.push_back(face);
            } else if (face.size() == 4) {
                quad_face.push_back(face);
            }
        }
    }
}
