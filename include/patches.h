#ifndef LAB_6_PATCHES_H
#define LAB_6_PATCHES_H

#include "mesh.h"

class Patches {
    // here the vertices for the level 0 lod are being saved
    std::vector<Vertex> lod_0_vertices;
    // here all 16 possible retriangulations for the level 0 patches will be saved
    // each retriangulation is an vector of indices
    std::vector<std::vector<GLuint>> lod_0_indices;

    // here the vertices for the level 1 lod are being saved
    std::vector<Vertex> lod_1_vertices;
    // here all 16 possible retriangulations for the level 1 patches will be saved
    // each retriangulation is an vector of indices
    std::vector<std::vector<GLuint>> lod_1_indices;

    // here the vertices for the level 2 lod are being saved
    std::vector<Vertex> lod_2_vertices;
    // here the indices for the level 2 lod are being saved
    std::vector<GLuint> lod_2_indices;

    GLuint VAO;
    // the VBO_LOD_0 Buffer will store the lod 0 vertices, the EBO_LOD_0 Buffer the lod 0 indices
    GLuint VBO_LOD_0, EBO_LOD_0;
    // the VBO_LOD_1 Buffer will store the lod 1 vertices, the EBO_LOD_1 Buffer the lod 1 indices
    GLuint VBO_LOD_1, EBO_LOD_1;
    // the VBO_LOD_2 Buffer will store the lod 2 vertices, the EBO_LOD_2 Buffer the lod 2 indices
    GLuint VBO_LOD_2, EBO_LOD_2;
    // the height_map_texture will be used inside the vertex shader to determine the height of each vertex
    GLuint height_map_texture;

    // here for each patch the current lod is being saved
    std::vector<std::vector<int>> patch_lods;

    // here for each patch the corresponding model matrix is being saved
    std::vector<std::vector<glm::mat4>> patch_model_matrices;

    unsigned int patches_x_dimension, patches_z_dimension;

    void initialize_vertices();
    void initialize_indices();

    void remove_vertex_and_retriangulate(std::vector<GLuint>& current_indices, GLuint index);
public:

    Patches(GLuint texture, unsigned int patches_x_dimension, unsigned int patches_z_dimension);

    void update_lod(glm::vec3 camera_position, bool fixed_lod, int selected_lod);

    // here all patches will be drawn
    void draw(GLuint shader, bool retriangulate);
};

#endif //LAB_6_PATCHES_H