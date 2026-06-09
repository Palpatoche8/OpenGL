#pragma once
#include <glad/glad.h>
#include "Math.hpp"
#include <array>

static constexpr int MAX_PARTICLES = 500;

struct Particle {
    Vec3  pos;
    Vec3  vel;
    float life;
    float maxLife;
    Vec3  spawnOrigin;
};

struct ParticleVertex {
    Vec3  pos;
    float life;
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void update(float dt);
    void uploadToGPU();

    GLuint vao()       const { return m_vao; }
    int    liveCount() const { return m_liveCount; }
    void setSpawnOrigin(Vec3 o) { m_spawnOrigin = o; }

private:
    std::array<Particle,       MAX_PARTICLES> m_pool;
    std::array<ParticleVertex, MAX_PARTICLES> m_verts;
    int   m_liveCount  = 0;
    float m_spawnAccum = 0.f;
    GLuint m_vao, m_vbo;
    void spawnOne();
    Vec3 m_spawnOrigin = {0.f, 0.f, 0.f};
};
