#include "../include/obj_loader.h"

//this obj loader is very basic and only creates one mesh
namespace ObjLoader {
    std::pair<std::vector<Vertex*>,std::vector<GLuint>> load_obj(std::string const &obj_path) {
        std::ifstream ifs(obj_path);
        if (!ifs.is_open()) {
            std::cout << "obj could not be loaded\n";
        }
        std::string current_line;

        std::vector<Vertex*> vertices;
        std::vector<GLuint> indices;

        while (std::getline(ifs, current_line)) {
            std::vector<std::string> lineParts;
            std::stringstream iss(current_line);
            std::string word;

            while (iss >> word) {
                lineParts.push_back(word);
            }

            if (lineParts.size() > 1) {
                if (lineParts[0] == "v") {
                    vertices.push_back(new Vertex{{stof(lineParts[1]), -stof(lineParts[3]), stof(lineParts[2])}});
                }


                if (lineParts[0] == "f") {
                    for (int i = 1; i < 4; ++i) {
                        std::string pos;
                        std::string norm;
                        std::string tex_coord;
                        int slash_count = 0;
                        for (char c : lineParts[i]) {
                            if (c == '/') {
                                slash_count++;
                            } else {
                                if (slash_count == 0)
                                    pos += c;
                                else if (slash_count == 1)
                                    tex_coord += c;
                                else if (slash_count == 2)
                                    norm += c;
                            }
                        }

                        indices.push_back(stof(pos) - 1);

                    }

                    if (lineParts.size() == 5) {
                        for (int i = 2; i < 5; ++i) {
                            std::string pos;
                            std::string norm;
                            std::string tex_coord;
                            int slash_count = 0;
                            int j = i;
                            if(i == 2)
                                j = 1;
                            for (char c : lineParts[j]) {
                                if (c == '/') {
                                    slash_count++;
                                } else {
                                    if (slash_count == 0)
                                        pos += c;
                                    else if (slash_count == 1)
                                        tex_coord += c;
                                    else if (slash_count == 2)
                                        norm += c;
                                }
                            }

                            indices.push_back(stof(pos) - 1);
                        }
                    }
                }
            }
        }

        return std::pair<std::vector<Vertex*>,std::vector<GLuint>>{vertices, indices};
    }
}
