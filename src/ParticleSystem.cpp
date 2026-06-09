#include "ParticleSystem.hpp"
#include <cmath>
#include <cstdlib>
#include <cstddef>

#define PI              3.14159265f
#define SPAWN_RADIUS    0.35f    // campfire base radius in world units
#define FIRE_BASE_Y    -1.4f    // matches model translate y
#define SPAWN_HEIGHT    -0.1f

#define HORIZONTAL_SPEED      0.06f
#define MIN_VERTICAL_SPEED    0.5f
#define VERTICAL_SPEED_RANGE  0.4f

#define MIN_LIFE    1.2f
#define LIFE_RANGE  0.6f

#define SPAWN_RATE          400.f
#define TURBULENCE          0.5f
#define HORIZONTAL_DAMPING  0.80f
#define FIRE_CURVE_SCALE    1.4f

static float randf()      { return (float)rand() / (float)RAND_MAX; }
static float randSigned() { return randf() * 2.f - 1.f; }

ParticleSystem::ParticleSystem()
{
    for (auto &p : m_pool) p.life = 0.f;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_verts), nullptr, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
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

static Vec3 randomFireSpawnPos()
{
    float angle = randf() * 2.f * PI;
    float r     = std::sqrt(randf()) * SPAWN_RADIUS;
    return Vec3(std::cos(angle) * r,
                FIRE_BASE_Y + randSigned() * SPAWN_HEIGHT,
                std::sin(angle) * r);
}

void ParticleSystem::spawnOne()
{
    for (auto &p : m_pool) {
        if (p.life <= 0.f) {
            p.pos     = randomFireSpawnPos();
            p.vel     = Vec3(randSigned() * HORIZONTAL_SPEED,
                             MIN_VERTICAL_SPEED + randf() * VERTICAL_SPEED_RANGE,
                             randSigned() * HORIZONTAL_SPEED);
            p.maxLife = MIN_LIFE + randf() * LIFE_RANGE;
            p.life    = p.maxLife;
            return;
        }
    }
}

void ParticleSystem::update(float dt)
{
    m_spawnAccum += dt * SPAWN_RATE;
    while (m_spawnAccum >= 1.f) { spawnOne(); m_spawnAccum -= 1.f; }
    m_liveCount = 0;
    for (auto &p : m_pool) {
        if (p.life <= 0.f) continue;
        p.life -= dt;
        if (p.life <= 0.f) continue;

        // Turbulent horizontal drift
        p.vel.x += randSigned() * TURBULENCE * dt;
        p.vel.z += randSigned() * TURBULENCE * dt;
        p.vel.x *= HORIZONTAL_DAMPING;
        p.vel.z *= HORIZONTAL_DAMPING;

        p.pos += p.vel * dt;

        float t = 1.f - (p.life / p.maxLife);  // 0=fresh, 1=dying

        // Narrow the flame toward top — gives a teardrop silhouette
        // (colleague's curve: width = sqrt(t) * sqrt(1-t), scaled)
        float width = FIRE_CURVE_SCALE * std::min(std::sqrt(t), std::sqrt(1.f - t));
        float pull  = (1.f - width) * dt * 5.f;
        p.pos.x -= p.pos.x * pull;
        p.pos.z -= p.pos.z * pull;

        m_verts[m_liveCount++] = { p.pos, t };
    }
}

void ParticleSystem::uploadToGPU()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    m_liveCount * sizeof(ParticleVertex), m_verts.data());
}
