#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    GLuint vert = compile(vertPath, GL_VERTEX_SHADER);
    GLuint frag = compile(fragPath, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);

    GLint ok;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(id, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() { glDeleteProgram(id); }

void Shader::setInt(const std::string& n, int v) const {
    glUniform1i(glGetUniformLocation(id, n.c_str()), v);
}
void Shader::setFloat(const std::string& n, float v) const {
    glUniform1f(glGetUniformLocation(id, n.c_str()), v);
}
void Shader::setVec2(const std::string& n, float x, float y) const {
    glUniform2f(glGetUniformLocation(id, n.c_str()), x, y);
}
void Shader::setVec3(const std::string& n, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(id, n.c_str()), x, y, z);
}
void Shader::setMat4(const std::string& n, const float* mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, n.c_str()), 1, GL_FALSE, mat);
}

GLuint Shader::compile(const std::string& path, GLenum type) {
    std::string src  = readFile(path);
    const char* csrc = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader compile error (") + path + "): " + log);
    }
    return shader;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open shader: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
