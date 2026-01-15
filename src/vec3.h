#ifndef VEC3_H
#define VEC3_H

#include "raytracer.h"

class vec3 {
    public:
        union {
            struct { double x, y, z; };
            double e[3];
        };

        vec3() : vec3(0) {}
        vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
        vec3(double d) : x(d), y(d), z(d) {}

        vec3 operator-() const { return vec3(-x, -y, -z); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

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

        vec3& operator*=(double t) {
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

        vec3& operator/=(double t) {
            return *this *= 1.0 / t;
        }

        vec3& operator=(const vec3& v) {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        double length() const {
            return std::sqrt(length_squared());
        }

        double length_squared() const {
            return x*x + y*y + z*z;
        }

        bool near_zero() const {
            double s = 1e-8;
            return std::fabs(x) < s && std::fabs(y) < s && std::fabs(z) < s;
        }

        vec3 dir() const;

        static vec3 random() {
            return vec3(random_double(), random_double(), random_double());
        }

        static vec3 random(double min, double max) {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
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

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.x, t * v.y, t * v.z);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1 / t) * v;
}

vec3 vec3::dir() const {
    return *this / length();
}

inline double dot(const vec3& u, const vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.z * v.z,
                u.x * v.y - u.y * v.x);
}

inline vec3 random_unit_vector() {
    while (true) {
        vec3 p = vec3::random(-1, 1);
        double lensq = p.length_squared();
        if (lensq > 1e-160 && lensq <= 1) return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 random_v = random_unit_vector();
    return (dot(random_v, normal) > 0.0) ? random_v : -random_v;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    // v is directed into the surface, n is directed out
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& v, const vec3& n, double index_ratio) {
    // v is directed in opposite direction from n
    double cos = dot(-v.dir(), n);
    vec3 r_perp = index_ratio * (v + v.length() * cos * n);
    vec3 r_para = -std::sqrt(1 - r_perp.length_squared()) * n;
    return r_perp + r_para;
}

#endif