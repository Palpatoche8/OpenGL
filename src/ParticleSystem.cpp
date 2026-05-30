#include "ParticleSystem.hpp"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <glad/glad.h>

static float randf()
{
    return (float)rand() / (float)RAND_MAX;
}

static float randSigned()
{
    return randf() * 2.f - 1.f;
}

ParticleSystem::ParticleSystem()
{
    for (auto &p : m_pool)
        p.life = 0.f;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_verts), nullptr, GL_STREAM_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                          (void *)offsetof(ParticleVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                          (void *)offsetof(ParticleVertex, life));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void ParticleSystem::spawnOne()
{
    for (auto &p : m_pool)
    {
        if (p.life <= 0.f)
        {
            p.pos = Vec2(randSigned() * 0.06f, -0.85f);

            float spreadX = randSigned() * 0.18f;
            float speedY = 0.5f + randf() * 0.6f;
            p.vel = Vec2(spreadX, speedY);

            p.maxLife = 1.2f + randf() * 0.8f;
            p.life = p.maxLife;
            return;
        }
    }
}

void ParticleSystem::update(float dt)
{
    m_spawnAccum += dt * 350.f;
    while (m_spawnAccum >= 1.f)
    {
        spawnOne();
        m_spawnAccum -= 1.f;
    }

    m_liveCount = 0;
    for (auto &p : m_pool)
    {
        if (p.life <= 0.f)
            continue;

        p.life -= dt;
        if (p.life <= 0.f)
            continue;

        p.vel.x += randSigned() * 0.04f;
        p.vel.x *= 0.97f;

        p.pos += p.vel * dt;

        float t = 1.f - (p.life / p.maxLife);
        m_verts[m_liveCount++] = { p.pos, t };
    }
}

void ParticleSystem::uploadToGPU()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_liveCount * sizeof(ParticleVertex),
                    m_verts.data());
}
