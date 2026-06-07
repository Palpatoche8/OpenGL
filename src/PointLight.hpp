#pragma once
#include "Math.hpp"
#include <cmath>

struct PointLight {
    Vec3  position  = {0.f, 0.4f, 0.f};
    Vec3  color     = {1.f, 0.45f, 0.05f};
    float intensity = 2.5f;

    void update(float time) {
        float flicker = 1.f
            + 0.12f * std::sin(time * 7.3f)
            + 0.08f * std::sin(time * 13.1f)
            + 0.05f * std::sin(time * 21.7f);
        intensity = 2.5f * flicker;
    }
};
