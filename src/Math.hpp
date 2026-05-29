#pragma once
#include <cmath>


struct Vec2 {
    float x = 0.f, y = 0.f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2  operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2  operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2  operator*(float s)       const { return {x * s,   y * s};   }
    Vec2  operator/(float s)       const { return {x / s,   y / s};   }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s)       { x *= s;   y *= s;   return *this; }

    float length()  const { return std::sqrt(x*x + y*y); }

    Vec2  normalized() const { 
        float l = length(); 
        if (l > 0) {
            return *this / l;
        }
        return Vec2{};
    }

    float dot(const Vec2& o) const { return x*o.x + y*o.y; }
};
inline Vec2 operator*(float s, const Vec2& v) { return v * s; }


struct Vec3 {
    float x = 0.f, y = 0.f, z = 0.f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    explicit Vec3(float v) : x(v), y(v), z(v) {}

    Vec3  operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3  operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3  operator*(float s)       const { return {x*s,   y*s,   z*s};   }
    Vec3  operator/(float s)       const { return {x/s,   y/s,   z/s};   }
    Vec3  operator-()              const { return {-x, -y, -z}; }
    Vec3& operator+=(const Vec3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
    Vec3& operator*=(float s)       { x*=s;   y*=s;   z*=s;   return *this; }

    float length()    const { return std::sqrt(x*x + y*y + z*z); }
    Vec3  normalized() const { 
        float l = length(); 
        if (l > 0) {
            return *this / l;
        }
        return Vec3{};
    }

    float dot(const Vec3& o)   const { return x*o.x + y*o.y + z*o.z; }
    Vec3  cross(const Vec3& o) const {
        return { y*o.z - z*o.y,
                 z*o.x - x*o.z,
                 x*o.y - y*o.x };
    }
};
inline Vec3 operator*(float s, const Vec3& v) { return v * s; }

