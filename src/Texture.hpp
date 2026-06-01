#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <iostream>

#include "image_io.hh"

static GLuint loadTGATexture(const char *path)
{
    tifo::rgb24_image *image = tifo::load_image(path);

    if (!image)
    {
        std::cerr << "Failed to load texture: " << path << '\n';
        return 0;
    }

    GLuint textureId;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->sx, image->sy, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    delete image;

    return textureId;
}

#endif