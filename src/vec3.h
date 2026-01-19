#ifndef VEC3_H
#define VEC3_H

#include "raytracer.h"

#include <string>
#include <regex>
#include <iostream>
#include <sstream>

class vec3 {
    public:
        union {
            struct { float x, y, z; };
            float e[3];
        };

        vec3() : vec3(0) {}
        vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
        vec3(float d) : x(d), y(d), z(d) {}

        vec3 operator-() const { return vec3(-x, -y, -z); }
        float operator[](int i) const { return e[i]; }
        float& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        vec3& operator*=(const vec3& v) {
            x *= v.x;
            y *= v.x;
            z *= v.x;
            return *this;
        }

        vec3& operator*=(float t) {
            x *= t;
            y *= t;
            z *= t;
            return *this;
        }

        vec3& operator/=(const vec3& v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        vec3& operator/=(float t) {
            return *this *= 1.0 / t;
        }

        vec3& operator=(const vec3& v) {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        float length() const {
            return std::sqrt(length_squared());
        }

        float length_squared() const {
            return x*x + y*y + z*z;
        }

        bool near_zero() const {
            float s = 1e-8;
            return std::fabs(x) < s && std::fabs(y) < s && std::fabs(z) < s;
        }

        vec3 dir() const;

        static vec3 random() {
            return vec3(random_float(), random_float(), random_float());
        }

        static vec3 random(float min, float max) {
            return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
        }

        static vec3 stov(std::string str) {
        vec3 out;
        static std::regex re(
            "^\\(\\s*(-?\\d+(\\.\\d+)?)\\s*,\\s*(-?\\d+(\\.\\d+)?)\\s*,\\s*(-?\\d+(\\.\\d+)?)\\s*\\)$"
        );
        if (std::regex_match(str, re)) {
            str.erase(0, 1);
            str.pop_back();

            std::stringstream ss(str);
            char comma;
            
            ss >> out.x >> comma >> out.y >> comma >> out.z;
        }
        return out;
    }
};

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(float t, const vec3& v) {
    return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, float t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, float t) {
    return (1 / t) * v;
}

vec3 vec3::dir() const {
    return *this / length();
}

inline float dot(const vec3& u, const vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

inline vec3 random_unit_vector() {
    while (true) {
        vec3 p = vec3::random(-1, 1);
        float lensq = p.length_squared();
        if (lensq > 1e-160 && lensq <= 1) return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 random_v = random_unit_vector();
    return (dot(random_v, normal) > 0.0) ? random_v : -random_v;
}

inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3(random_float(-1, 1), random_float(-1, 1), 0);
        if (p.length_squared() < 1) return p;
    }
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    // v is directed into the surface, n is directed out
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& v, const vec3& n, float index_ratio) {
    // v is directed in opposite direction from n
    float cos = dot(-v.dir(), n);
    vec3 r_perp = index_ratio * (v + v.length() * cos * n);
    vec3 r_para = -std::sqrt(1 - r_perp.length_squared()) * n;
    return r_perp + r_para;
}

#endif