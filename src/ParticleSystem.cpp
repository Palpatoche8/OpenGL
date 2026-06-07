#include "ParticleSystem.hpp"

#define PI 3.14159265f

#define SPAWN_RADIUS 0.6f
#define FIRE_BASE_Y -1.4f
#define SPAWN_HEIGHT 0.02f

#define HORIZONTAL_SPEED 0.08f
#define MIN_VERTICAL_SPEED 0.5f
#define VERTICAL_SPEED_RANGE 0.4f

#define MIN_LIFE 1.2f
#define LIFE_RANGE 0.6f

#define SPAWN_RATE 10000.f
#define TURBULENCE 0.9f
#define HORIZONTAL_DAMPING 0.7f
#define FIRE_CURVE_SCALE 1.4f

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

Vec3 randomFireSpawnPos(float radius)
{
    float angle = randf() * 2.0f * PI;
    float r = std::sqrt(randf()) * radius;

    return Vec3(std::cos(angle) * r, FIRE_BASE_Y + randSigned() * SPAWN_HEIGHT,
                std::sin(angle) * r);
}

void ParticleSystem::spawnOne()
{
    for (auto &p : m_pool)
    {
        if (p.life <= 0.f)
        {
            p.pos = randomFireSpawnPos(SPAWN_RADIUS);

            float speedY = MIN_VERTICAL_SPEED + randf() * VERTICAL_SPEED_RANGE;

            p.vel = Vec3(randSigned() * HORIZONTAL_SPEED, speedY,
                         randSigned() * HORIZONTAL_SPEED);

            p.maxLife = MIN_LIFE + randf() * LIFE_RANGE;
            p.life = p.maxLife;
            return;
        }
    }
}

void ParticleSystem::update(float dt)
{
    m_spawnAccum += dt * SPAWN_RATE;
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

        float oldT = 1.f - p.life / p.maxLife;

        p.life -= dt;
        if (p.life <= 0.f)
            continue;

        p.vel.x += randSigned() * TURBULENCE;
        p.vel.z += randSigned() * TURBULENCE;

        p.vel.x *= HORIZONTAL_DAMPING;
        p.vel.z *= HORIZONTAL_DAMPING;

        p.pos += p.vel * dt;

        float t = 1.f - (p.life / p.maxLife);

        // distribution made by me !
        // https://www.desmos.com/calculator/gcppjhpcj5
        float width =
            FIRE_CURVE_SCALE * std::min(std::sqrt(t), std::sqrt(1.0f - t));

        float pull = (1.0f - width) * dt * 5.0f;

        p.pos.x -= p.pos.x * pull;
        p.pos.z -= p.pos.z * pull;

        m_verts[m_liveCount++] = { p.pos, t };
    }
}

void ParticleSystem::uploadToGPU()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_liveCount * sizeof(ParticleVertex),
                    m_verts.data());
}