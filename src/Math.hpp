#pragma once
#include <cmath>

struct Vec2
{
    float x = 0.f, y = 0.f;

    Vec2() = default;
    Vec2(float x, float y)
        : x(x)
        , y(y)
    {}

    Vec2 operator+(const Vec2 &o) const
    {
        return { x + o.x, y + o.y };
    }
    Vec2 operator-(const Vec2 &o) const
    {
        return { x - o.x, y - o.y };
    }
    Vec2 operator*(float s) const
    {
        return { x * s, y * s };
    }
    Vec2 operator/(float s) const
    {
        return { x / s, y / s };
    }
    Vec2 &operator+=(const Vec2 &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }
    Vec2 &operator-=(const Vec2 &o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }
    Vec2 &operator*=(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    Vec2 normalized() const
    {
        float l = length();
        if (l > 0)
        {
            return *this / l;
        }
        return Vec2{};
    }

    float dot(const Vec2 &o) const
    {
        return x * o.x + y * o.y;
    }
};

inline Vec2 operator*(float s, const Vec2 &v)
{
    return v * s;
}

struct Vec3
{
    float x = 0.f, y = 0.f, z = 0.f;

    Vec3() = default;
    Vec3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {}
    explicit Vec3(float v)
        : x(v)
        , y(v)
        , z(v)
    {}

    Vec3 operator+(const Vec3 &o) const
    {
        return { x + o.x, y + o.y, z + o.z };
    }
    Vec3 operator-(const Vec3 &o) const
    {
        return { x - o.x, y - o.y, z - o.z };
    }
    Vec3 operator*(float s) const
    {
        return { x * s, y * s, z * s };
    }
    Vec3 operator/(float s) const
    {
        return { x / s, y / s, z / s };
    }
    Vec3 operator-() const
    {
        return { -x, -y, -z };
    }
    Vec3 &operator+=(const Vec3 &o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
    Vec3 &operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    float length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    Vec3 normalized() const
    {
        float l = length();
        if (l > 0)
        {
            return *this / l;
        }
        return Vec3{};
    }

    float dot(const Vec3 &o) const
    {
        return x * o.x + y * o.y + z * o.z;
    }
    Vec3 cross(const Vec3 &o) const
    {
        return { y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x };
    }
};

inline Vec3 operator*(float s, const Vec3 &v)
{
    return v * s;
}

struct Mat4
{
    float m[16];
};

static Mat4 identity()
{
    Mat4 r = {};
    r.m[0] = 1.f;
    r.m[5] = 1.f;
    r.m[10] = 1.f;
    r.m[15] = 1.f;
    return r;
}

static Mat4 perspective(float fovDeg, float aspect, float nearPlane,
                        float farPlane)
{
    float fovRad = fovDeg * M_PI / 180.f;
    float f = 1.f / std::tan(fovRad / 2.f);

    Mat4 r = {};
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    r.m[11] = -1.f;
    r.m[14] = (2.f * farPlane * nearPlane) / (nearPlane - farPlane);

    return r;
}

static Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up)
{
    Vec3 f = (center - eye).normalized();
    Vec3 s = f.cross(up).normalized();
    Vec3 u = s.cross(f);

    Mat4 r = identity();

    r.m[0] = s.x;
    r.m[4] = s.y;
    r.m[8] = s.z;

    r.m[1] = u.x;
    r.m[5] = u.y;
    r.m[9] = u.z;

    r.m[2] = -f.x;
    r.m[6] = -f.y;
    r.m[10] = -f.z;

    r.m[12] = -s.dot(eye);
    r.m[13] = -u.dot(eye);
    r.m[14] = f.dot(eye);

    return r;
}