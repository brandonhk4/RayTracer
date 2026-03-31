#ifndef QUAT_H
#define QUAT_H

#include "raytracer.h"

class quat {
    public:
        float s;
        vec3 v;

        static quat eye;

        quat() : quat(0.0f) {}
        quat(float s) : s(s), v(0.0f) {}
        quat(const vec3& v) : s(0.0f), v(v) {}
        quat(float x, float y, float z) : s(0.0f), v(x, y, z) {}
        quat(float s, vec3 v) : s(s), v(v) {}
        quat(float s, float x, float y, float z) : s(s), v(x, y, z) {}

        quat operator-() const { return quat(-s, -v); }
        float operator[](int i) const { if (!i) return s; return v[i]; }
        float& operator[](int i) { if (!i) return s; return v[i]; }

        quat& operator+=(const quat& q) {
            s += q.s;
            v += q.v;
            return *this;
        }

        quat& operator-=(const quat& q) {
            s -= q.s;
            v -= q.v;
            return *this;
        }

        quat& operator*=(const quat& q) {
            float temps = s;
            s = s * q.s - dot(v, q.v);
            v = temps * q.v + q.s * v + cross(v, q.v);
            return *this;
        }

        quat& operator*=(float t) {
            s *= t;
            v *= t;
            return *this;
        }

        quat& operator=(const quat& q) {
            s = q.s;
            v = q.v;
            return *this;
        }

        float norm() const {
            return std::sqrt(norm_squared());
        }

        float norm_squared() const {
            return s * s + v.length_squared();
        }

        quat normalized() const;

        quat unit() const {
            return quat(0.0f, v.dir());
        }

        quat conjugate() const {
            return quat(s, -v);
        }

        quat inv() const;

        vec3 rotate(const vec3& p) const;
};

quat quat::eye(0.0f, 0.0f, 0.0f, 1.0f);

inline quat operator+(const quat& p, const quat& q) {
    return quat(p.s + q.s, p.v + q.v);
}

inline quat operator-(const quat& p, const quat& q) {
    return quat(p.s - q.s, p.v - q.v);
}

inline quat operator*(const quat& p, const quat& q) {
    return quat(p.s * q.s - dot(p.v, q.v), p.s * q.v + q.s * p.v + cross(p.v, q.v));
}

inline quat operator*(float t, const quat& q) {
    return quat(t * q.s, t * q.v);
}

inline quat operator*(const quat& q, float t) {
    return t * q;
}

inline quat operator/(const quat& p, const quat& q) {
    return p * q.inv();
}

inline quat operator/(const quat& q, float t) {
    return (1.0f / t) * q;
}

quat quat::normalized() const {
    return *this / norm();
}

quat quat::inv() const {
    return conjugate() / norm_squared();
}

inline float dot(const quat& p, const quat& q) {
    return p.s * q.s + dot(p.v, q.v);
}

vec3 rotate(const vec3& point, const vec3& axis, float theta, bool radians=false) {
    if (!radians) theta = degrees_to_radians(theta);
    
    quat q(std::cos(theta / 2), std::sin(theta / 2) * axis);
    quat p(point);
    return (q * p * q.inv()).v;
}

vec3 quat::rotate(const vec3& p) const {
    return (*this * quat(p) * inv()).v;
}

quat rotate_to(const vec3& point, const vec3& target) {
    vec3 u = point.dir();
    vec3 v = target.dir();
    float cos_theta = dot(u, v);
    vec3 axis = cross(u, v);

    float cos_half_theta = std::sqrt((cos_theta + 1.0f) / 2.0f);
    float sin_half_theta = std::sqrt((1.0f - cos_theta) / 2.0f);

    return quat(cos_half_theta, sin_half_theta * axis);
}

class dquat {
    public:
        quat p;
        quat d;

        static dquat eye;

        dquat() : dquat(0.0f, 0.0f) {}
        dquat(float pf, float df) : p(pf), d(df) {}
        dquat(const quat& p, const quat& d) : p(p), d(d) {}
        dquat(float ps, const vec3& pv, float ds, const vec3& dv) : p(ps, pv), d(ds, dv) {}
        dquat(float e0, float e1, float e2, float e3, float e4, float e5, float e6, float e7) :
              p(e0, e1, e2, e3), d(e4, e5, e6, e7) {}
        // dquat(const dual& s, const dvec3& v) : p(s.r, v.r()), d(s.d, v.d()) {}
        
        dquat operator-() const { return dquat(-p, -d); }
        float operator[](int i) const { if (i < 4) return p[i]; return d[i - 4]; }
        float& operator[](int i) { if (i < 4) return p[i]; return d[i - 4]; }

        dquat& operator+=(const dquat& dq) {
            p += dq.p;
            d += dq.d;
            return *this;
        }

        dquat& operator-=(const dquat& dq) {
            p -= dq.p;
            d -= dq.d;
            return *this;
        }

        dquat& operator*=(const dquat& dq) {
            quat temps = p;
            p = p * dq.p;
            d = temps * dq.d + d * dq.p;
            return *this;
        }

        dquat& operator*=(float t) {
            p *= t;
            d *= t;
            return *this;
        }

        dquat& operator=(const dquat& dq) {
            p = dq.p;
            d = dq.d;
            return *this;
        }

        dquat norm() const {
            return dquat(p.norm_squared(), 2.0f * dot(p, d));
        }

        dquat inv() const;

        bool isUnit() const {
            return std::fabs(dot(p, p) - 1.0f ) < 1e-8 && std::fabs(dot(p, d)) < 1e-8;
        }

        dquat piece_conjugate() const {
            return dquat(p.conjugate(), d.conjugate());
        }

        dquat eps_conjugate() const {
            return dquat(p, -d);
        }

        dquat conjugate() const {
            return dquat(p.s, -p.v, -d.s, d.v);
        }

        vec3 transform(const vec3& p) const;
        
        ray transform(const ray& r) const;

        static dquat rotate(float theta, const vec3& axis);
        
        static dquat translate(const vec3& offset);
};

dquat dquat::eye = dquat(1.0f, 0.0f);

inline dquat operator+(const dquat& p, const dquat& q) {
    return dquat(p.p + q.p, p.d + q.d);
}

inline dquat operator-(const dquat& p, const dquat& q) {
    return dquat(p.p - q.p, p.d - q.d);
}

inline dquat operator*(const dquat& p, const dquat& q) {
    return dquat(p.p * q.p, p.p * q.d + p.d * q.p);
}

dquat dquat::inv() const {
    return dquat(p.inv(), quat()) * dquat(quat(1.0f), -d * p.inv());
}

inline dquat dquat::rotate(float theta, const vec3& axis) {
    return dquat(quat(std::cos(theta / 2.0f), std::sin(theta / 2.0f) * axis.dir()), quat());
}

inline dquat dquat::translate(const vec3& offset) {
    return dquat(quat(1.0f), quat(offset / 2.0f));
}

vec3 dquat::transform(const vec3& p) const {
    return (*this * dquat(quat(1.0f), quat(p)) * conjugate()).d.v;
}

ray dquat::transform(const ray& r) const {
    return ray(transform(r.pt()), p.rotate(r.dir()), r.time());
}

#endif