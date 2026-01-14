#ifndef RAY_H
#define RAY_H

#include "raytracer.h"

class ray {
    private:
        vec3 pt;
        vec3 dir;
    
    public:
        ray() {}

        ray(const vec3& point, const vec3& direction) : pt(point), dir(direction) {}

        const vec3& point() const  { return pt; }
        const vec3& direction() const { return dir; }

        vec3 at(double t) const {
            return pt + t * dir;
        }
};

#endif