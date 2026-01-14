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
            y += v.x;
            z += v.x;
            return *this;
        }

        vec3& operator*=(const vec3& v) {
            x *= v.x;
            y *= v.x;
            z *= v.x;
            return *this;
        }

        vec3& operator*=(double t) {
            x += t;
            y += t;
            z += t;
            return *this;
        }

        vec3& operator/=(const vec3& v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        vec3& operator/=(double t) {
            *this *= 1 / t;
            return *this;
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

        vec3 dir() const;
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

#endif