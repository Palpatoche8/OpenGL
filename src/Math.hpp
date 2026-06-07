#pragma once
#include <cmath>

// ---------------------------------------------------------------------------
// Vec2 — Phase 1+
// ---------------------------------------------------------------------------
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
    Vec2  normalized() const { float l = length(); return l > 0 ? *this / l : Vec2{}; }
    float dot(const Vec2& o) const { return x*o.x + y*o.y; }
};
inline Vec2 operator*(float s, const Vec2& v) { return v * s; }

// ---------------------------------------------------------------------------
// Vec3 — Phase 2+
// ---------------------------------------------------------------------------
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
    Vec3& operator-=(const Vec3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
    Vec3& operator*=(float s)       { x*=s;   y*=s;   z*=s;   return *this; }

    float length()    const { return std::sqrt(x*x + y*y + z*z); }
    Vec3  normalized() const { float l = length(); return l > 0 ? *this / l : Vec3{}; }

    float dot(const Vec3& o)   const { return x*o.x + y*o.y + z*o.z; }
    Vec3  cross(const Vec3& o) const {
        return { y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x };
    }
};
inline Vec3 operator*(float s, const Vec3& v) { return v * s; }

// ---------------------------------------------------------------------------
// Vec4 — Phase 2+
// ---------------------------------------------------------------------------
struct Vec4 {
    float x = 0.f, y = 0.f, z = 0.f, w = 0.f;

    Vec4() = default;
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(const Vec3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec4  operator+(const Vec4& o) const { return {x+o.x, y+o.y, z+o.z, w+o.w}; }
    Vec4  operator*(float s)       const { return {x*s,   y*s,   z*s,   w*s};   }
    Vec4& operator+=(const Vec4& o) { x+=o.x; y+=o.y; z+=o.z; w+=o.w; return *this; }

    Vec3 xyz() const { return {x, y, z}; }
};

// ---------------------------------------------------------------------------
// Mat4 — Phase 2+ — column-major (OpenGL convention)
// ---------------------------------------------------------------------------
struct Mat4 {
    float m[16] = {};   // m[col*4 + row]

    static Mat4 identity() {
        Mat4 r;
        r.m[0]=1; r.m[5]=1; r.m[10]=1; r.m[15]=1;
        return r;
    }

    const float* ptr() const { return m; }

    Mat4 operator*(const Mat4& o) const {
        Mat4 r;
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row) {
                float s = 0;
                for (int k = 0; k < 4; ++k)
                    s += m[k*4+row] * o.m[col*4+k];
                r.m[col*4+row] = s;
            }
        return r;
    }

    Vec4 operator*(const Vec4& v) const {
        return {
            m[0]*v.x + m[4]*v.y + m[8]*v.z  + m[12]*v.w,
            m[1]*v.x + m[5]*v.y + m[9]*v.z  + m[13]*v.w,
            m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]*v.w,
            m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15]*v.w,
        };
    }

    static Mat4 translate(const Vec3& t) {
        Mat4 r = identity();
        r.m[12]=t.x; r.m[13]=t.y; r.m[14]=t.z;
        return r;
    }

    static Mat4 scale(const Vec3& s) {
        Mat4 r = identity();
        r.m[0]=s.x; r.m[5]=s.y; r.m[10]=s.z;
        return r;
    }

    static Mat4 rotate(float angle, Vec3 axis) {
        axis = axis.normalized();
        float c = std::cos(angle), s = std::sin(angle), t = 1-c;
        float x=axis.x, y=axis.y, z=axis.z;
        Mat4 r = identity();
        r.m[0]=t*x*x+c;   r.m[4]=t*x*y-s*z; r.m[8] =t*x*z+s*y;
        r.m[1]=t*x*y+s*z; r.m[5]=t*y*y+c;   r.m[9] =t*y*z-s*x;
        r.m[2]=t*x*z-s*y; r.m[6]=t*y*z+s*x; r.m[10]=t*z*z+c;
        return r;
    }

    static Mat4 perspective(float fovY, float aspect, float nearP, float farP) {
        float f = 1.f / std::tan(fovY * 0.5f);
        Mat4 r;
        r.m[0]  =  f / aspect;
        r.m[5]  =  f;
        r.m[10] =  (farP + nearP) / (nearP - farP);
        r.m[11] = -1.f;
        r.m[14] =  (2.f * farP * nearP) / (nearP - farP);
        return r;
    }

    static Mat4 ortho(float l, float r_, float b, float t_, float nearP, float farP) {
        Mat4 res = identity();
        res.m[0]  =  2.f/(r_-l);
        res.m[5]  =  2.f/(t_-b);
        res.m[10] = -2.f/(farP-nearP);
        res.m[12] = -(r_+l)/(r_-l);
        res.m[13] = -(t_+b)/(t_-b);
        res.m[14] = -(farP+nearP)/(farP-nearP);
        return res;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalized();
        Vec3 s = f.cross(up).normalized();
        Vec3 u = s.cross(f);
        Mat4 r = identity();
        r.m[0]=s.x;  r.m[4]=s.y;  r.m[8] =s.z;
        r.m[1]=u.x;  r.m[5]=u.y;  r.m[9] =u.z;
        r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
        r.m[12]=-s.dot(eye); r.m[13]=-u.dot(eye); r.m[14]=f.dot(eye);
        return r;
    }
};
