#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "raytracer.h"

class sphere : public hittable {
    private:
        const vec3 center;    
        const float radius;
        shared_ptr<material> mat;

    public:    
        sphere(const vec3& cen, float rad, shared_ptr<material> mat) : 
            center(cen), radius(std::fmax(0, rad)), mat(mat) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = center - r.pt();
            float a = r.dir().length_squared();
            float h = dot(r.dir(), oc);
            float c = oc.length_squared() - radius * radius;

            float discriminant = h * h - a * c;
            if (discriminant < 0) return false;
            
            float sqrtd = std::sqrt(discriminant);
            float root = (h - sqrtd) / a;
            
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            rec.t = root;
            rec.pt = r.at(root);
            rec.normal = (rec.pt - center) / radius;
            rec.mat = mat;

            return true;
        }
};

#endif