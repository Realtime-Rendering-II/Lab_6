#ifndef LAB_4_LIGHT_H
#define LAB_4_LIGHT_H

#include <glm/glm.hpp>

struct Light {
public:
    glm::vec3 position;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;

    float strength;

    float computed_position[3];

    Light(glm::vec3 pos, glm::vec3 dif_col, glm::vec3 spec_col, float str) : position(pos),
                                                                             diffuse_color(dif_col),
                                                                             specular_color(spec_col), strength(str) {
        model_matrix = glm::mat4{1.0f};
        computed_position[0] = pos.x;
        computed_position[1] = pos.y;
        computed_position[2] = pos.z;
        setup();
    }

    void draw(GLuint shader) {
        glUseProgram(shader);

        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model_matrix[0][0]);
        glBindVertexArray(VAO);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (GLvoid *) 0);

        glDrawArrays(GL_POINTS, 0, 1);

        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void setup() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &computed_position[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glm::vec3 get_computed_position(){
        return glm::vec3{computed_position[0], computed_position[1], computed_position[2]};
    }

    glm::mat4 get_model_matrix(){
        return model_matrix;
    }

    void set_model_matrix(glm::mat4 const& new_model_matrix){
        model_matrix = new_model_matrix;
        glm::vec4 pos = model_matrix * glm::vec4{position.x, position.y, position.z,1};
        computed_position[0] = pos.x;
        computed_position[1] = pos.y;
        computed_position[2] = pos.z;
    }

private:
    glm::mat4 model_matrix;
    GLuint VAO,VBO;
};

#endif //LAB_4_LIGHT_H