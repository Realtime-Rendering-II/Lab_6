#include "../include/mesh.h"

Mesh::Mesh(std::vector<Vertex*> vertices_in, std::vector<GLuint> indices) : indices(indices) {
    for(Vertex* vert : vertices_in){
        vertices.push_back(*vert);
    }
    setup_mesh();
    model_matrix = glm::mat4(1.0f);
}

void Mesh::draw(GLuint shader) {
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model_matrix[0][0]);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *) 0);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void Mesh::setup_mesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::mat4 Mesh::get_model_matrix() {
    return model_matrix;
}

void Mesh::set_model_matrix(glm::mat4 new_model) {
    model_matrix = new_model;
}

std::vector<Vertex> Mesh::get_vertices() {
    return vertices;
}


