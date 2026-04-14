#ifndef MAT3_H
#define MAT3_H

#include "vec3.h"

class mat3 {
    public:
        union {
            float e[9];
            vec3 v[3];
        };

        static mat3 eye;

        mat3() : mat3(0.0f) {}
        mat3(float d) {
            v[0] = vec3(d);
            v[1] = vec3(d);
            v[2] = vec3(d);
            v[3] = vec3(d);
        }
        mat3(const vec3& v0, const vec3& v1, const vec3& v2) {
            v[0] = v0;
            v[1] = v1;
            v[2] = v2;
        }
        mat3(float e0, float e1, float e2,
             float e3, float e4, float e5,
             float e6, float e7, float e8)
        {
            e[0] = e0;
            e[1] = e1;
            e[2] = e2;
            e[3] = e3;
            e[4] = e4;
            e[5] = e5;
            e[6] = e6;
            e[7] = e7;
            e[8] = e8;
        }
        
        mat3 operator-() const { return mat3(-v[0], -v[1], -v[2]); }
        vec3 operator[](int i) const { return v[i]; }
        vec3& operator[](int i) { return v[i]; }

        mat3& operator+=(const mat3& m) {
            for (int i = 0; i < 9; ++i) {
                e[i] += m.e[i];
            }
            return *this;
        }

        mat3& operator*=(const mat3& m) {
            for (int i = 0; i < 9; ++i) {
                e[i] *= m.e[i];
            }
            return *this;
        }

        mat3& operator*=(float t) {
            for (int i = 0; i < 9; ++i) {
                e[i] *= t;
            }
            return *this;
        }

        mat3& operator/=(const mat3& m) {
            for (int i = 0; i < 9; ++i) {
                e[i] /= m.e[i];
            }
            return *this;
        }

        mat3& operator/=(float t) {
            return *this *= 1.0 / t;
        }

        mat3& operator=(const mat3& m) {
            for (int i = 0; i < 9; ++i) {
                e[i] = m.e[i];
            }
            return *this;
        }

        float det() const {
            return e[0] * e[4] * e[8] +
                   e[1] * e[5] * e[6] +
                   e[2] * e[3] * e[7] -
                   e[0] * e[5] * e[7] -
                   e[1] * e[3] * e[8] -
                   e[2] * e[4] * e[6];
        }

        mat3 transpose() const {
            mat3 ret;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    ret[i][j] = v[j][i];
                }
            }
            return ret;
        }

        mat3 x(const mat3& m) const {
            return mat3(m.v[0] * v[0], m.v[1] * v[1], m.v[2] * v[2]);
        }
};

mat3 mat3::eye
    (
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

inline std::ostream& operator<<(std::ostream& out, const mat3& m) {
    return out << m.v[0] << '\n' << m.v[1] << '\n' << m.v[2];
}

inline mat3 operator+(const mat3& m, const mat3& n) {
    return mat3(m.v[0] + n.v[0], m.v[1] + n.v[1], m.v[2] + n.v[2]);
}

inline mat3 operator-(const mat3& m, const mat3& n) {
    return mat3(m.v[0] - n.v[0], m.v[1] - n.v[1], m.v[2] - n.v[2]);
}

inline mat3 operator*(float t, const mat3& m) {
    return mat3(t * m.v[0], t * m.v[1], t * m.v[2]);
}

inline mat3 operator*(const mat3& m, float t) {
    return t * m;
}

inline vec3 operator*(const mat3& m, const vec3& v) {
    return vec3(dot(m.v[0], v), dot(m.v[1], v), dot(m.v[2], v));
}

inline vec3 operator*(const vec3& v, const mat3& m) {
    return m.transpose() * v;
}

inline mat3 operator*(const mat3& m, const mat3& n) {
    mat3 nT = n.transpose();
    mat3 ret;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ret[i][j] = dot(m[i], nT[j]);
        }
    }
    return ret;
}

inline mat3 operator/(const mat3& m, float t) {
    return (1.0f / t) * m;
}

#endif