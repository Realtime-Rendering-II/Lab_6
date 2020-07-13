#ifndef LAB_4_OBJ_LOADER_H
#define LAB_4_OBJ_LOADER_H

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "mesh.h"
#include "structs.h"

namespace ObjLoader{
    std::pair<std::vector<Vertex*>,std::vector<GLuint>> load_obj(std::string const& obj_path);
};

#endif //LAB_4_OBJ_LOADER_H
