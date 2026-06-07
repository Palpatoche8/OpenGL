#pragma once
#include <glad/glad.h>
#include "Math.hpp"
#include <array>

static constexpr int MAX_SMOKE = 60;

struct SmokeParticle {
    Vec3  pos;
    Vec3  vel;
    float life;
    float maxLife;
};

struct SmokeVertex {
    Vec3  pos;
    float life;
};

class SmokeSystem {
public:
    SmokeSystem();
    ~SmokeSystem();
    void update(float dt);
    void uploadToGPU();
    GLuint vao()       const { return m_vao; }
    int    liveCount() const { return m_liveCount; }
private:
    std::array<SmokeParticle, MAX_SMOKE> m_pool;
    std::array<SmokeVertex,   MAX_SMOKE> m_verts;
    int   m_liveCount  = 0;
    float m_spawnAccum = 0.f;
    GLuint m_vao, m_vbo;
    void spawnOne();
};
