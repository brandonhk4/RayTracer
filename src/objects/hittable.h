#ifndef HITTABLE_H
#define HITTABLE_H

#include "../math/ray.h"
#include "../utility/bbox.h"

class material;

class hit_record {
    public:
        vec3 pt;
        vec3 normal;
        shared_ptr<material> mat;
        float t;
        float u;
        float v;
};

class hittable {
    public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual bbox bounding_box() const = 0;

    virtual bool empty() const { return true; }

    virtual float pdf_value(const vec3& origin, const vec3& direction) const {
        return 0.0f;
    }

    virtual vec3 random(const vec3& origin) const {
        return vec3(1.0f, 0.0f, 0.0f);
    }
};

#endif