#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform sampler2D height_map;

uniform int width;
uniform int length;

out vec3 color;

void main(){
    vec4 vertex_position = model * vec4(in_position,1);
    float grey_value = texture(height_map, vec2(vertex_position.x/width,vertex_position.z/length)).g;
    vertex_position.y = grey_value;
    gl_Position = projection * view * vertex_position;

    color = vec3(grey_value,grey_value,grey_value);
}