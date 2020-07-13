#ifndef LAB_4_TEXTURE_H
#define LAB_4_TEXTURE_H

#include <GL/glew.h>
#include "stb_image.h"
#include <string>
#include <iostream>

class Texture{
public:
    Texture(std::string const& texture_path);
    ~Texture();
    GLuint get_texture();
private:
    void load_texture(std::string const& texture_path);
    GLuint texture;
};

#endif //LAB_4_TEXTURE_H
