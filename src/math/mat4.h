#ifndef MAT4_H
#define MAT4_H

#include "vec4.h"

class mat4 {
    public:
        union {
            float e[16];
            vec4 v[4];
        };

        static mat4 eye;

        mat4() : mat4(0.0f) {}
        mat4(float d) {
            v[0] = vec4(d);
            v[1] = vec4(d);
            v[2] = vec4(d);
            v[3] = vec4(d);
        }
        mat4(vec4 v0, vec4 v1, vec4 v2, vec4 v3) {
            v[0] = v0;
            v[1] = v1;
            v[2] = v2;
            v[3] = v3;
        }
        mat4(float e0, float e1, float e2, float e3, 
             float e4, float e5, float e6, float e7, 
             float e8, float e9, float e10, float e11, 
             float e12, float e13, float e14, float e15)
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
            e[9] = e9;
            e[10] = e10;
            e[11] = e11;
            e[12] = e12;
            e[13] = e13;
            e[14] = e14;
            e[15] = e15;
        }
        
        mat4 operator-() const { return mat4(-v[0], -v[1], -v[2], -v[3]); }
        vec4 operator[](int i) const { return v[i]; }
        vec4& operator[](int i) { return v[i]; }

        mat4& operator+=(const mat4& m) {
            for (int i = 0; i < 16; ++i) {
                e[i] += m.e[i];
            }
            return *this;
        }

        mat4& operator*=(const mat4& m) {
            for (int i = 0; i < 16; ++i) {
                e[i] *= m.e[i];
            }
            return *this;
        }

        mat4& operator*=(float t) {
            for (int i = 0; i < 16; ++i) {
                e[i] *= t;
            }
            return *this;
        }

        mat4& operator/=(const mat4& m) {
            for (int i = 0; i < 16; ++i) {
                e[i] /= m.e[i];
            }
            return *this;
        }

        mat4& operator/=(float t) {
            return *this *= 1.0 / t;
        }

        mat4& operator=(const mat4& m) {
            for (int i = 0; i < 16; ++i) {
                e[i] = m.e[i];
            }
            return *this;
        }

        float det() const {
            return e[0] * e[5] * e[10] * e[15] +
                   e[1] * e[6] * e[11] * e[12] +
                   e[2] * e[7] * e[8]  * e[13] +
                   e[3] * e[4] * e[9]  * e[14] -
                   e[0] * e[7] * e[10] * e[13] -
                   e[1] * e[4] * e[11] * e[14] -
                   e[2] * e[5] * e[8]  * e[15] -
                   e[3] * e[6] * e[9]  * e[12]; 
        }

        mat4 transpose() const {
            mat4 ret;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    ret[i][j] = v[j][i];
                }
            }
            return ret;
        }

        mat4 x(const mat4& m) const {
            return mat4(m.v[0] * v[0], m.v[1] * v[1], m.v[2] * v[2], m.v[3] * v[3]);
        }
};

mat4 mat4::eye
    (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

inline std::ostream& operator<<(std::ostream& out, const mat4& m) {
    return out << m.v[0] << '\n' << m.v[1] << '\n' << m.v[2] << '\n' << m.v[3];
}

inline mat4 operator+(const mat4& m, const mat4& n) {
    return mat4(m.v[0] + n.v[0], m.v[1] + n.v[1], m.v[2] + n.v[2], m.v[3] + n.v[3]);
}

inline mat4 operator-(const mat4& m, const mat4& n) {
    return mat4(m.v[0] - n.v[0], m.v[1] - n.v[1], m.v[2] - n.v[2], m.v[3] - n.v[3]);
}

inline mat4 operator*(float t, const mat4& m) {
    return mat4(t * m.v[0], t * m.v[1], t * m.v[2], t * m.v[3]);
}

inline mat4 operator*(const mat4& m, float t) {
    return t * m;
}

inline vec4 operator*(const mat4& m, const vec4& v) {
    return vec4(dot(m.v[0], v), dot(m.v[1], v), dot(m.v[2], v), dot(m.v[3], v));
}

inline vec4 operator*(const vec4& v, const mat4& m) {
    return m.transpose() * v;
}

inline mat4 operator*(const mat4& m, const mat4& n) {
    mat4 nT = n.transpose();
    mat4 ret;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ret[i][j] = dot(m[i], nT[j]);
        }
    }
    return ret;
}

inline mat4 operator/(const mat4& m, float t) {
    return (1.0f / t) * m;
}

#endif