#include "SmokeSystem.hpp"
#include <cstdlib>
#include <cstddef>

static float rf()  { return (float)rand() / (float)RAND_MAX; }
static float rsf() { return rf() * 2.f - 1.f; }

SmokeSystem::SmokeSystem() {
    for (auto& p : m_pool) p.life = 0.f;
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_verts), nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SmokeVertex),
                          (void*)offsetof(SmokeVertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SmokeVertex),
                          (void*)offsetof(SmokeVertex, life));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

SmokeSystem::~SmokeSystem() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void SmokeSystem::spawnOne() {
    for (auto& p : m_pool) {
        if (p.life <= 0.f) {
            p.pos = Vec3(rsf() * 0.1f, 1.0f + rf() * 0.4f, rsf() * 0.1f);
            p.vel = Vec3(rsf() * 0.05f, 0.15f + rf() * 0.15f, rsf() * 0.05f);
            p.maxLife = 3.5f + rf() * 2.5f;
            p.life    = p.maxLife;
            return;
        }
    }
}

void SmokeSystem::update(float dt) {
    m_spawnAccum += dt * 8.f;
    while (m_spawnAccum >= 1.f) { spawnOne(); m_spawnAccum -= 1.f; }
    m_liveCount = 0;
    for (auto& p : m_pool) {
        if (p.life <= 0.f) continue;
        p.life -= dt;
        if (p.life <= 0.f) continue;
        p.vel.x += rsf() * 0.005f;
        p.vel.z += rsf() * 0.005f;
        p.pos = p.pos + p.vel * dt;
        float t = 1.f - (p.life / p.maxLife);
        m_verts[m_liveCount++] = { p.pos, t };
    }
}

void SmokeSystem::uploadToGPU() {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_liveCount * sizeof(SmokeVertex), m_verts.data());
}
