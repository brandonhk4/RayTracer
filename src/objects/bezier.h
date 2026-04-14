#ifndef BEZIER_H
#define BEZIDER_H

#include "../math/vec3.h"
#include "../math/mat4.h"
#include "triangle.h"
#include "patch.h"
#include "hittable_list.h"
#include "material.h"

#include <iostream>

const mat4 basis
    (
        -1.0f,  3.0f, -3.0f, 1.0f,
         3.0f, -6.0f,  3.0f, 0.0f,
        -3.0f,  3.0f,  0.0f, 0.0f,
         1.0f,  0.0f,  0.0f, 0.0f
    );

class bezier_curve {
    private:
        vec4 x, y, z;

        vec3 dBezier(float t) const {
            vec4 tvec(3.0f * t * t, 2.0f * t, 1.0f, 0.0f);
            return vec3(dot(tvec, x), dot(tvec, y), dot(tvec, z));
        }

    public:
        bezier_curve(vec3 p0, vec3 p1, vec3 p2, vec3 p3) :
                     x(vec4(p0.x, p1.x, p2.x, p3.x) * basis), 
                     y(vec4(p0.y, p1.y, p2.y, p3.y) * basis), 
                     z(vec4(p0.z, p1.z, p2.z, p3.z) * basis) {}

        vec3 at(float t) const {
            vec4 tvec(t*t*t, t*t, t, 1.0f);
            return vec3(dot(tvec, x), dot(tvec, y), dot(tvec, z));
        }

        std::vector<vec3> split(int div) const{
            std::vector<vec3> ret;
            ret.reserve(div + 1);
            for (float i = 0; i < div + 1; ++i) {
                ret.push_back(i / div);
            }
            return ret;
        }
};

class bezier_patch {
    private:
        mat4 x, y, z;

    public:
        bezier_patch(const mat4& x, const mat4& y, const mat4& z) : 
                    x(basis*x*basis), y(basis*y*basis), z(basis*z*basis) {}

        bezier_patch(const vec3& v0,  const vec3& v1,  const vec3& v2,  const vec3& v3,
                     const vec3& v4,  const vec3& v5,  const vec3& v6,  const vec3& v7,
                     const vec3& v8,  const vec3& v9,  const vec3& v10, const vec3& v11,
                     const vec3& v12, const vec3& v13, const vec3& v14, const vec3& v15) :
                    x(basis * mat4(v0.x,  v1.x,  v2.x,  v3.x,
                                   v4.x,  v5.x,  v6.x,  v7.x,
                                   v8.x,  v9.x,  v10.x, v11.x,
                                   v12.x, v13.x, v14.x, v15.x) * basis),
                    y(basis * mat4(v0.y,  v1.y,  v2.y,  v3.y,
                                   v4.y,  v5.y,  v6.y,  v7.y,
                                   v8.y,  v9.y,  v10.y, v11.y,
                                   v12.y, v13.y, v14.y, v15.y) * basis),
                    z(basis * mat4(v0.z,  v1.z,  v2.z,  v3.z,
                                   v4.z,  v5.z,  v6.z,  v7.z,
                                   v8.z,  v9.z,  v10.z, v11.z,
                                   v12.z, v13.z, v14.z, v15.z) * basis) {}
        
        vec3 at(float u, float v) const {
            vec4 uvec(u*u*u, u*u, u, 1.0f);
            vec4 vvec(v*v*v, v*v, v, 1.0f);
            return vec3(dot(vvec, x * uvec), dot(vvec, y * uvec), dot(vvec, z * uvec));
        }

        vec3 normal(float u, float v) const {
            vec4 uvec(u*u*u, u*u, u, 1.0f);
            vec4 vvec(v*v*v, v*v, v, 1.0f);
            vec4 duvec(3.0f * u * u, 2.0f * u, 1.0f, 0.0f);
            vec4 dvvec(3.0f * u * u, 2.0f * u, 1.0f, 0.0f);
            vec3 du(dot(vvec, x * duvec), dot(vvec, y * duvec), dot(vvec, z * duvec));
            vec3 dv(dot(dvvec, x * uvec), dot(dvvec, y * uvec), dot(dvvec, z * uvec));
            return cross(du, dv).dir();
        }

        std::vector<vec3> split(int div) const {
            std::vector<vec3> pts;
            pts.reserve((div + 1) * (div + 1));
            
            for (float i = 0.0f; i < div + 1; ++i) {
                for (float j = 0.0f; j < div + 1; ++j) {
                    pts.push_back(at(i / div, j / div));   
                }
            }

            return pts;
        }

        hittable_list tesselate(int div, shared_ptr<material> mat) const {
            // hittable_list ret(div * div * 2);

            // std::vector<vec3> pt = split(div);
            // for (int i = 0; i < div; ++i) {
            //     for (int j = 0; j < div; ++j) {
            //         vec3 Q = pt[j + i * (div + 1)];
            //         vec3 a = pt[j + 1 + i * (div + 1)];
            //         vec3 b = pt[j + (i + 1) * (div + 1)];
            //         vec3 R = pt[j + 1 + (i + 1) * (div + 1)];
            //         ret.add(make_shared<triangle>(Q, a, b, mat));
            //         ret.add(make_shared<triangle>(R, b, a, mat));
            //     }
            // }

            hittable_list ret(div * div);

            std::vector<vec3> pt = split(div);
            for (int i = 0; i < div; ++i) {
                for (int j = 0; j < div; ++j) {
                    vec3 p0 = pt[j + i * (div + 1)];
                    vec3 p1 = pt[j + (i + 1) * (div + 1)];
                    vec3 p2 = pt[j + 1 + i * (div + 1)];
                    vec3 p3 = pt[j + 1 + (i + 1) * (div + 1)];
                    ret.add(make_shared<patch>(p0, p1, p2, p3, mat));
                }
            }
            return ret;
        }
};

#endif