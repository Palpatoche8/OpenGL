#include "ParticleSystem.hpp"

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

void ParticleSystem::spawnOne(const std::vector<std::string> &pattern)
{
    std::vector<Vec2> cells;

    for (int y = 0; y < (int)pattern.size(); ++y)
    {
        for (int x = 0; x < (int)pattern[y].size(); ++x)
        {
            if (pattern[y][x] == 'X')
                cells.push_back(Vec2((float)x, (float)y));
        }
    }

    if (cells.empty())
        return;

    for (auto &p : m_pool)
    {
        if (p.life <= 0.f)
        {
            Vec2 cell = cells[rand() % cells.size()];

            float cellSize = 0.08f;

            float patternWidth = (float)pattern[0].size();
            float patternHeight = (float)pattern.size();

            float x = (cell.x - patternWidth * 0.5f) * cellSize;
            float y = -(cell.y - patternHeight * 0.5f) * cellSize;

            x += randSigned() * cellSize * 0.35f;
            y += randSigned() * cellSize * 0.35f;

            p.pos = Vec3(x, randSigned() * 0.03f - 1, -0.85f + y);

            float spreadX = randSigned() * 0.18f;
            float speedY = 0.2f + randf() * 0.5f;
            p.vel = Vec3(spreadX, speedY, 0.f);

            p.maxLife = 1.2f + randf() * 0.8f;
            p.life = p.maxLife;
            return;
        }
    }
}

std::string s1 = "  X     X  ";
std::string s2 = "   X   X   ";
std::string s3 = "  XXXXXXX  ";
std::string s4 = " XX XXX XX ";
std::string s5 = "XXXXXXXXXXX";
std::string s6 = "X XXXXXXX X";
std::string s7 = "X X     X X";
std::string s8 = "   XX XX   ";

std::string *pattern = new std::string[8]{ s1, s2, s3, s4, s5, s6, s7, s8 };

void ParticleSystem::update(float dt)
{
    m_spawnAccum += dt * 10000.f;
    while (m_spawnAccum >= 1.f)
    {
        spawnOne(std::vector<std::string>(pattern, pattern + 8));
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
