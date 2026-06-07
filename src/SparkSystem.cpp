#include "SparkSystem.hpp"
#include <cstdlib>
#include <cstddef>
#include <cmath>

static float rf()  { return (float)rand() / (float)RAND_MAX; }
static float rsf() { return rf() * 2.f - 1.f; }

SparkSystem::SparkSystem() {
    for (auto& s : m_pool) s.life = 0.f;
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_verts), nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SparkVertex),
                          (void*)offsetof(SparkVertex, pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SparkVertex),
                          (void*)offsetof(SparkVertex, life));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

SparkSystem::~SparkSystem() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void SparkSystem::spawnOne() {
    for (auto& s : m_pool) {
        if (s.life <= 0.f) {
            s.pos = Vec3(rsf() * 0.12f, 0.1f + rf() * 0.3f, rsf() * 0.12f);
            s.vel = Vec3(rsf() * 0.8f, 2.0f + rf() * 2.5f, rsf() * 0.8f);
            s.maxLife = 0.4f + rf() * 0.5f;
            s.life    = s.maxLife;
            return;
        }
    }
}

void SparkSystem::update(float dt) {
    m_spawnAccum += dt * 60.f;
    while (m_spawnAccum >= 1.f) { spawnOne(); m_spawnAccum -= 1.f; }
    m_liveCount = 0;
    for (auto& s : m_pool) {
        if (s.life <= 0.f) continue;
        s.life -= dt;
        if (s.life <= 0.f) continue;
        s.vel.y -= 5.0f * dt;
        s.vel.x *= 0.98f;
        s.vel.z *= 0.98f;
        s.pos = s.pos + s.vel * dt;
        if (s.pos.y < 0.01f && s.vel.y < 0.f) {
            s.pos.y = 0.01f;
            s.vel.y *= -0.25f;
        }
        float t = 1.f - (s.life / s.maxLife);
        m_verts[m_liveCount++] = { s.pos, t };
    }
}

void SparkSystem::uploadToGPU() {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_liveCount * sizeof(SparkVertex), m_verts.data());
}
