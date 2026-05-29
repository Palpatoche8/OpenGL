#pragma once
#include <glad/glad.h>
#include "Math.hpp"
#include <array>

static constexpr int MAX_PARTICLES = 500;

struct Particle {
    Vec2  pos;
    Vec2  vel;
    float life;   
    float maxLife;   
};

struct ParticleVertex {
    Vec2  pos;
    float life; 
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void update(float dt);
    void uploadToGPU();

    GLuint vao() const { return m_vao; }
    int    liveCount() const { return m_liveCount; }

private:
    std::array<Particle, MAX_PARTICLES> m_pool;
    std::array<ParticleVertex, MAX_PARTICLES> m_verts;
    int m_liveCount = 0;

    GLuint m_vao, m_vbo;

    float m_spawnAccum = 0.f;

    void spawnOne();
};
