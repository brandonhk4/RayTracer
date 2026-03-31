#ifndef VEC4_H
#define VEC4_H

#include "../raytracer.h"

class vec4 {
    public:
        union {
            struct { float x, y, z, w; };
            float e[4];
        };

        vec4() : vec4(0) {}
        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        vec4(float d) : x(d), y(d), z(d), w(d) {}
        vec4(const vec3& v, float w = 0.0f) : x(v.x), y(v.y), z(v.z), w(w) {}

        vec4 operator-() const { return vec4(-x, -y, -z, -w); }
        float operator[](int i) const { return e[i]; }
        float& operator[](int i) { return e[i]; }

        vec4& operator+=(const vec4& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        vec4& operator*=(const vec4& v) {
            x *= v.x;
            y *= v.x;
            z *= v.x;
            w *= v.w;
            return *this;
        }

        vec4& operator*=(float t) {
            x *= t;
            y *= t;
            z *= t;
            w *= t;
            return *this;
        }

        vec4& operator/=(const vec4& v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }

        vec4& operator/=(float t) {
            return *this *= 1.0 / t;
        }

        vec4& operator=(const vec4& v) {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }

        float length() const {
            return std::sqrt(length_squared());
        }

        float length_squared() const {
            return x*x + y*y + z*z + w*w;
        }

        vec4 dir() const;

        static vec4 random() {
            return vec4(random_float(), random_float(), random_float(), random_float());
        }

        static vec4 random(float min, float max) {
            return vec4(random_float(min, max), random_float(min, max), random_float(min, max), random_float(min, max));
        }
};

inline bool near_zero(const vec4& v) {
    float s = 1e-8;
    return std::fabs(v.x) < s && std::fabs(v.y) < s && std::fabs(v.z) < s && std::fabs(v.w) < s;
}

inline std::ostream& operator<<(std::ostream& out, const vec4& v) {
    return out << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
}

inline vec4 operator+(const vec4& u, const vec4& v) {
    return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

inline vec4 operator-(const vec4& u, const vec4& v) {
    return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

inline vec4 operator*(const vec4& u, const vec4& v) {
    return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

inline vec4 operator*(float t, const vec4& v) {
    return vec4(t * v.x, t * v.y, t * v.z, t * v.w);
}

inline vec4 operator*(const vec4& v, float t) {
    return t * v;
}

inline vec4 operator/(const vec4& v, float t) {
    return (1.0f / t) * v;
}

vec4 vec4::dir() const {
    return *this / length();
}

inline float dot(const vec4& u, const vec4& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

#endif