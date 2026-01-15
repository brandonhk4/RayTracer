#ifndef RAY_H
#define RAY_H

#include "raytracer.h"

class ray {
    private:
        vec3 point;
        vec3 direction;
    
    public:
        ray() {}

        ray(const vec3& point, const vec3& direction) : point(point), direction(direction) {}

        const vec3& pt() const  { return point; }
        const vec3& dir() const { return direction; }

        vec3 at(double t) const {
            return point + t * direction;
        }
};

#endif