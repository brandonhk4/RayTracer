#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "bbox.h"

class material;

class hit_record {
    public:
        vec3 pt;
        vec3 normal;
        shared_ptr<material> mat;
        float t;
};

class hittable {
    public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual bbox bounding_box() const = 0;
};

#endif