#ifndef LAB_4_MESH_H
#define LAB_4_MESH_H

#include <vector>
#include "structs.h"
#include <GL/glew.h>
#include <glm/matrix.hpp>

class Mesh{
public:
    Mesh(std::vector<Vertex*> vertices, std::vector<GLuint> indices);
    void draw(GLuint shader);
    glm::mat4 get_model_matrix();

    virtual void set_model_matrix(glm::mat4 new_model);
    std::vector<Vertex> get_vertices();
protected:
    GLuint VAO, VBO, EBO;
    glm::mat4 model_matrix;
    void setup_mesh();
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};
#endif //LAB_4_MESH_H
