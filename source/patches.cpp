#include "../include/patches.h"
#include <glm/gtx/transform.hpp>
#include <set>

// the patches_x_dimension defines how many patches will be next to each other in x direction
// the patches_y_dimension defines how many patches will be next to each other in y direction
Patches::Patches(GLuint texture, unsigned int patches_x_dimension, unsigned int patches_z_dimension) :
        height_map_texture(texture),
        patches_z_dimension(patches_z_dimension),
        patches_x_dimension(patches_x_dimension){

    // TODO 1: implement the function initialize_vertices
    initialize_vertices();

    // TODO 2: implement the function initialize_indices
    initialize_indices();

    for(int x = 0; x < patches_x_dimension; ++x){
        std::vector<int> row_lod;
        std::vector<glm::mat4> row_model_matrices;
        for(int z = 0; z < patches_z_dimension; ++z){
            row_lod.push_back(2);
            glm::mat4 current_model_matrix{1.0f};
            current_model_matrix = glm::translate(current_model_matrix, glm::vec3{x,0,z});
            row_model_matrices.push_back(current_model_matrix);
        }
        patch_lods.push_back(row_lod);
        patch_model_matrices.push_back(row_model_matrices);
    }

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO_LOD_0);
    glGenBuffers(1, &EBO_LOD_0);
    glGenBuffers(1, &VBO_LOD_1);
    glGenBuffers(1, &EBO_LOD_1);
    glGenBuffers(1, &VBO_LOD_2);
    glGenBuffers(1, &EBO_LOD_2);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD_0);
    glBufferData(GL_ARRAY_BUFFER, lod_0_vertices.size() * sizeof(Vertex), lod_0_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD_0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lod_0_indices[0].size() * sizeof(GLuint), lod_0_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD_1);
    glBufferData(GL_ARRAY_BUFFER, lod_1_vertices.size() * sizeof(Vertex), lod_1_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD_1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lod_1_indices[0].size() * sizeof(GLuint), lod_1_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD_2);
    glBufferData(GL_ARRAY_BUFFER, lod_2_vertices.size() * sizeof(Vertex), lod_2_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD_2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lod_2_indices.size() * sizeof(GLuint), lod_2_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// depending on the distance between the patch and the camera a loevel of detail is assigned for the patch
void Patches::update_lod(glm::vec3 camera_position, bool fixed_lod, int selected_lod) {
    for(int i = 0; i < patch_model_matrices.size(); ++i){
        for(int j = 0; j < patch_model_matrices[0].size(); ++j){
            glm::mat4 patch_model_matrix = patch_model_matrices[i][j];
            glm::vec3 patch_center = glm::vec3{patch_model_matrix[3][0], patch_model_matrix[3][1], patch_model_matrix[3][2]};
            float distance_to_camera = glm::distance(camera_position, patch_center);
            if ((!fixed_lod && distance_to_camera < 3.0f) || (fixed_lod && selected_lod == 0))
                patch_lods[i][j] = 0;
            else if ((!fixed_lod && distance_to_camera < 6.0f) || (fixed_lod && selected_lod == 1))
                patch_lods[i][j] = 1;
            else
                patch_lods[i][j] = 2;
        }
    }
}

void Patches::draw(GLuint shader,  bool retriangulate) {
    glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, height_map_texture);
    glUniform1i(glGetUniformLocation(shader, "height_map"), 0);

    glUniform1i(glGetUniformLocation(shader, "width"), patches_x_dimension);
    glUniform1i(glGetUniformLocation(shader, "length"), patches_z_dimension);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);

    for(int i = 0; i < patch_lods.size(); ++i){
        for(int j = 0; j < patch_lods[0].size(); ++j){
            std::vector<GLuint> current_indices{};

            glm::mat4 current_model_matrix = patch_model_matrices[i][j];
            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &current_model_matrix[0][0]);

            int index = 0;

            if(retriangulate) {
                if (j  + 1 < patches_z_dimension && patch_lods[i][j + 1] > patch_lods[i][j]) {
                    index += 1;
                }

                if (j > 0 && patch_lods[i][j - 1] > patch_lods[i][j]) {
                    index += 2;
                }

                if (i > 0 && patch_lods[i - 1][j] > patch_lods[i][j]) {
                    index += 4;
                }

                if (i  + 1 < patches_x_dimension && patch_lods[i + 1][j] > patch_lods[i][j]) {
                    index += 8;
                }
            }

            if(patch_lods[i][j] == 0){
                glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD_0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD_0);

                // check if the triangulation with the current index can be accessed if not use the base triangulation
                if(lod_0_indices.size() > index)
                    current_indices = lod_0_indices[index];
                else
                    current_indices = lod_0_indices[0];
            } else if (patch_lods[i][j] == 1){
                glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD_1);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD_1);

                // check if the triangulation with the current index can be accessed if not use the base triangulation
                if(lod_1_indices.size() > index)
                    current_indices = lod_1_indices[index];
                else
                    current_indices = lod_1_indices[0];
            } else {
                glBindBuffer(GL_ARRAY_BUFFER, VBO_LOD_2);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_LOD_2);

                current_indices = lod_2_indices;
            }

            if(!current_indices.empty()) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, current_indices.size() * sizeof(GLuint), current_indices.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
                glDrawElements(GL_TRIANGLES, current_indices.size(), GL_UNSIGNED_INT, (void *) 0);
            }
        }
    }

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

// TODO 2.1: implement the function remove_vertex_and_retriangulate
// the function takes an vector of indices that encodes triangle and an index that defines which triangles will have to be
// removed and retriagulated. I.e. if we have the index buffer 0,1,2,3,4,5,2,1,3 it encodes the triangles 0,1,2 3,4,5 and 2,1,3
// if the index_to_remove is 1 than we want to remove the triangles 0,1,2 and 2,1,3 and retriangulate afterwards
// in this case we remove 0,1,2 and 2,1,3 and add the new triangle 0,2,3 to the index vector
void Patches::remove_vertex_and_retriangulate(std::vector<GLuint>& current_indices, GLuint index_to_remove) {
    std::set<int> new_triangle_indices;
    for(int i = 0; i < current_indices.size(); i += 3){
        if(current_indices[i] == index_to_remove || current_indices[i + 1] == index_to_remove || current_indices[i + 2] == index_to_remove){
            if(current_indices[i] != index_to_remove)
                new_triangle_indices.insert(current_indices[i]);
            if(current_indices[i + 1] != index_to_remove)
                new_triangle_indices.insert(current_indices[i + 1]);
            if(current_indices[i + 2] != index_to_remove)
                new_triangle_indices.insert(current_indices[i + 2]);
            current_indices.erase(current_indices.begin() + i);
            current_indices.erase(current_indices.begin() + i);
            current_indices.erase(current_indices.begin() + i);
            i -= 3;
        }
    }
    for(auto index : new_triangle_indices){
        current_indices.push_back(index);
    }
}

// TODO 1: implement the function initialize_vertices
// here the vertices for the lod_0_vertices, the lod_1_vertices and the lod_2_vertices vector are being initialized
// the x,y and z value of the vertices should be between 0 and 1
// the lod 0 representation will consist out of 25 vertices and 32 triangles
// the lod 1 representation will consist out of 9 vertices and 8 triangles
// the lod 2 representation will consist out of 4 vertices and 2 triangles
void Patches::initialize_vertices() {
    // Example for the level 2 vertices:
    // generate the vertices for the lod 2 representation
    // since the level 2 patch consists out of 1 square per length and width the values are chosen accordingly
    int squares_per_length = 1, squares_per_width = 1;
    for (int i = 0; i <= squares_per_width; i++) {
        for (int j = 0; j <= squares_per_length; j++) {
            lod_2_vertices.push_back(Vertex{glm::vec3{i * 1/squares_per_width,  0, j * 1/squares_per_length}});
        }
    }
}

// TODO 2: implement the function initialize_indices
// here the indices for the different triangulations will be computed
// for the lod 2 patch only 1 triangulation exists
// for the lod 1 patch and the lod 0 patch 16 different triangulations are possible (depending on the lod of the neighbor patches)
// to create the different triangulations implement and use the function remove_vertex_and_retriangulate
void Patches::initialize_indices() {
    // Example for the level 2 indices:
    // computation of the level 2 indices/ the lod 2 patch triangulation
    // since the level 2 patch consists out of 1 square per length and width the values are chosen accordingly
    int squares_per_length = 1, squares_per_width = 1;
    int a = 0, b = 1;
    for (int i = 0; i < squares_per_length; ++i) {
        for (int j = 0; j < squares_per_width; ++j) {
            lod_2_indices.push_back(a + j * (squares_per_length + 1) + i);
            lod_2_indices.push_back(b + j * (squares_per_length + 1) + i);
            lod_2_indices.push_back(a + (j + 1) * (squares_per_length + 1) + i);
            lod_2_indices.push_back(b + j * (squares_per_length + 1) + i);
            lod_2_indices.push_back(a + (j + 1) * (squares_per_length + 1) + i);
            lod_2_indices.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
        }
    }

    // here the base triangulation for the lod 1 patch is defined
    // this base triangulation can be modified if necessary
    squares_per_length = 2, squares_per_width = 2;
    bool last_d = false;
    a = 0; b = 1;
    std::vector<GLuint> lod_1_base_triangulation;
    for (int i = 0; i < squares_per_length; ++i) {
        for (int j = 0; j < squares_per_width; ++j) {
            if(last_d) {
                lod_1_base_triangulation.push_back(a + j * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(b + j * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(b + j * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
                last_d = !last_d;
            } else {
                lod_1_base_triangulation.push_back(a + j * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(b + j * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
                lod_1_base_triangulation.push_back(a + j * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i);
                lod_1_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
                last_d = !last_d;
            }
        }
        last_d = !last_d;
    }

    lod_1_indices.push_back(lod_1_base_triangulation);

    // TODO 2.2: create all possible triangulations for the lod 1 patch and save each triangulation in the lod_1_indices vector so that it can be accessed later
    // for this implement and use the function remove_vertex_and_retriangulation
    // the binary representation of the index i defines if the lod of the top,bottom,left or right neighbor is lower
    // if the index is 0 the binary representation is 0000.
    // the last bit defines if the top neighbor has a lower lod
    // the second last bit defines if the bottom neighbor has a lower lod
    // the third last bit defines if the left neighbor has a lower lod
    // the fourth last bit defines if the right neighbor has a lower lod
    // i.e. the index 5 is equal to 0101 and refers to the triangulation where the top and left neighbor have a lower lod
    // i.e. the index 2 is equal to 0010 and refers to the triangulation where the bottom neighbor has a lower lod
    for(int i = 1; i < 16; ++i){
        // check if first bit is set
        bool top = i & 1;
        bool bottom = i & 2;
        bool left = i & 4;
        bool right = i & 8;

        std::vector<GLuint> base_triangulation = lod_1_indices[0];

    }

    // here the base triangulation for the lod 0 patch is defined
    // this base triangulation can be modified if necessary
    squares_per_length = 4, squares_per_width = 4;
    last_d = false;
    a = 0; b = 1;
    std::vector<GLuint> lod_0_base_triangulation;
    for (int i = 0; i < squares_per_length; ++i) {
        for (int j = 0; j < squares_per_width; ++j) {
            if(last_d) {
                lod_0_base_triangulation.push_back(a + j * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(b + j * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(b + j * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
                last_d = !last_d;
            } else {
                lod_0_base_triangulation.push_back(a + j * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(b + j * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
                lod_0_base_triangulation.push_back(a + j * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i);
                lod_0_base_triangulation.push_back(a + (j + 1) * (squares_per_length + 1) + i + 1);
                last_d = !last_d;
            }
        }
        last_d = !last_d;
    }
    lod_0_indices.push_back(lod_0_base_triangulation);


    // TODO 2.3: create all possible triangulations for the lod 0 patch and save each triangulation in the lod_0_indices vector so that it can be accessed later
    // the binary representation of the index i defines if the lod of the top,bottom,left or right neighbor is lower
    // if the index is 0 the binary representation (b3 b2 b1 b0) is 0000.
    // the last bit (b0) defines if the top neighbor has a lower lod
    // the second last bit (b1) defines if the bottom neighbor has a lower lod
    // the third last bit defines (b2) if the left neighbor has a lower lod
    // the fourth last bit (b3) defines if the right neighbor has a lower lod
    // i.e. the index 5 is equal to 0101 and refers to the triangulation where the top and left neighbor have lower lod
    //      we know this because b0 and b3 are 1 and this the top and left neighbour have a lower lod
    //      in this case the top and left side of the patch will need to be retriangulated
    // i.e. the index 2 is equal to 0010 and refers to the triangulation where the bottom neighbor has a lower lod
    for(int i = 1; i < 16; ++i){
        // check if first bit is set
        bool top = i & 1;
        bool bottom = i & 2;
        bool left = i & 4;
        bool right = i & 8;

        std::vector<GLuint> base_triangulation = lod_0_indices[0];

    }

}
