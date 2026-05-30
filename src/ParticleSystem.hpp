#pragma once
#include <array>
#include <glad/glad.h>

#include "Math.hpp"

static constexpr int MAX_PARTICLES = 500;

struct Particle
{
    Vec3 pos;
    Vec3 vel;
    float life;
    float maxLife;
};

struct ParticleVertex
{
    Vec3 pos;
    float life;
};

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    void update(float dt);
    void uploadToGPU();

    GLuint vao() const
    {
        return m_vao;
    }
    int liveCount() const
    {
        return m_liveCount;
    }

private:
    std::array<Particle, MAX_PARTICLES> m_pool;
    std::array<ParticleVertex, MAX_PARTICLES> m_verts;
    int m_liveCount = 0;

    GLuint m_vao, m_vbo;

    float m_spawnAccum = 0.f;

    void spawnOne();
};
