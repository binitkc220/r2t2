#ifndef PARSE_OBJ_H_
#define PARSE_OBJ_H_

#include <string>
#include "../renderer/vec3.h"

void readObjFile(std::string s, std::vector<vec3> &vertices, std::vector<std::vector<int>> &triangle_face, std::vector<std::vector<int>> &quad_face);

#endif // PARSE_OBJ_H_
