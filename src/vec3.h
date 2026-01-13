#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class vec3 {
    public:
        double e[3];
        double& x = e[0];
        double& y = e[1];
        double& z = e[2];

        vec3() : e{0, 0, 0}{}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

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