#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
public:
    GLuint id;

    Shader(const std::string &vertPath, const std::string &fragPath);
    ~Shader();

    void use() const
    {
        glUseProgram(id);
    }

    void setFloat(const std::string &name, float v) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setMat4(const std::string &name, const float *mat) const;

private:
    static GLuint compile(const std::string &path, GLenum type);
    static std::string readFile(const std::string &path);
};
