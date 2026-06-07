#pragma once
#include <glad/glad.h>
#include "Math.hpp"
#include "Shader.hpp"

class Skybox {
public:
    Skybox();
    ~Skybox();
    void draw(const Mat4& proj, const Mat4& view);
private:
    GLuint m_vao, m_vbo;
    Shader m_shader;
};
