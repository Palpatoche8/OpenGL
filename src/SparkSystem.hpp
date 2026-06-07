#pragma once
#include <glad/glad.h>
#include "Math.hpp"
#include <array>

static constexpr int MAX_SPARKS = 200;

struct Spark {
    Vec3  pos;
    Vec3  vel;
    float life;
    float maxLife;
};

struct SparkVertex {
    Vec3  pos;
    float life;
};

class SparkSystem {
public:
    SparkSystem();
    ~SparkSystem();
    void update(float dt);
    void uploadToGPU();
    GLuint vao()       const { return m_vao; }
    int    liveCount() const { return m_liveCount; }
private:
    std::array<Spark,       MAX_SPARKS> m_pool;
    std::array<SparkVertex, MAX_SPARKS> m_verts;
    int   m_liveCount  = 0;
    float m_spawnAccum = 0.f;
    GLuint m_vao, m_vbo;
    void spawnOne();
};
