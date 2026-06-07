#include "Mesh.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>

struct ObjIndex
{
    int pos    = 0;
    int normal = 0;
};

static ObjIndex parseVertex(const std::string &s)
{
    ObjIndex index;
    size_t first  = s.find('/');
    size_t second = s.find('/', first + 1);

    if (first == std::string::npos) {
        index.pos = std::stoi(s);
        return index;
    }

    index.pos = std::stoi(s.substr(0, first));
    if (second != std::string::npos && second + 1 < s.size())
        index.normal = std::stoi(s.substr(second + 1));

    return index;
}

Mesh::Mesh(const std::string &path)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Cannot open OBJ: " << path << '\n';
        return;
    }

    std::vector<Vec3>       positions;
    std::vector<Vec3>       normals;
    std::vector<MeshVertex> vertices;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            Vec3 p; ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        } else if (type == "vn") {
            Vec3 n; ss >> n.x >> n.y >> n.z;
            normals.push_back(n.normalized());
        } else if (type == "f") {
            std::string a, b, c;
            ss >> a >> b >> c;

            ObjIndex ia = parseVertex(a);
            ObjIndex ib = parseVertex(b);
            ObjIndex ic = parseVertex(c);

            Vec3 pa = positions[ia.pos - 1];
            Vec3 pb = positions[ib.pos - 1];
            Vec3 pc = positions[ic.pos - 1];

            Vec3 faceNormal = (pb - pa).cross(pc - pa).normalized();

            Vec3 na = (ia.normal > 0) ? normals[ia.normal - 1] : faceNormal;
            Vec3 nb = (ib.normal > 0) ? normals[ib.normal - 1] : faceNormal;
            Vec3 nc = (ic.normal > 0) ? normals[ic.normal - 1] : faceNormal;

            vertices.push_back({ pa, na });
            vertices.push_back({ pb, nb });
            vertices.push_back({ pc, nc });
        }
    }

    std::cout << "Mesh loaded: " << vertices.size() << " vertices from " << path << '\n';
    upload(vertices);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Mesh::upload(const std::vector<MeshVertex> &vertices)
{
    m_count = (int)vertices.size();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                          (void *)offsetof(MeshVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex),
                          (void *)offsetof(MeshVertex, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    if (m_count == 0) return;
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_count);
    glBindVertexArray(0);
}
