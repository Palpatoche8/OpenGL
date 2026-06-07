#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

#include "Math.hpp"

struct MeshVertex
{
    Vec3 pos;
    Vec3 normal;
};

class Mesh
{
public:
    Mesh() = default;
    explicit Mesh(const std::string &path);

    void draw();

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    int m_count = 0;

    void upload(const std::vector<MeshVertex> &vertices);
};