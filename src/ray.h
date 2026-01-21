#ifndef RAY_H
#define RAY_H

#include "raytracer.h"

class ray {
    private:
        vec3 point;
        vec3 direction;
        float tm;
    
    public:
        ray() {}

        ray (const vec3& point, const vec3& direction, float time) :
            point(point), direction(direction), tm(time) {}

        ray(const vec3& point, const vec3& direction) : ray(point, direction, 0) {}

        const vec3& pt() const  { return point; }
        const vec3& dir() const { return direction; }
        float time() const { return tm; }

        vec3 at(float t) const {
            return point + t * direction;
        }
};

#endif