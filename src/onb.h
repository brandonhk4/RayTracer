#ifndef ONB_H
#define ONB_H

#include "vec3.h"

class onb {
    public:
        union{
            struct{
                vec3 u, v, w;
            };
            vec3 axis[3];
        };

        onb(const vec3& n) {
            w = n.dir();
            if (w.z < -0.999999f) {
                u = vec3(0.0f, -1.0f, 0.0f);
                v = vec3(-1.0f, 0.0f, 0.0f);
            } else {
                const float a = 1.0f / (1.0f + w.z);
                const float b = -w.x * w.y * a;
                u = vec3(1.0f - w.x * w.x * a, b, -w.x);
                v = vec3(b, 1.0f - w.y * w.y * a, -w.y);
            }
        }

        vec3 transform(const vec3& n) const {
            return u * n.x + v * n.y + w * n.z;
        }
};

#endif