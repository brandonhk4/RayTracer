#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "raytracer.h"

class sphere : public hittable {
    private:
        const ray center;    
        const float radius;
        shared_ptr<material> mat;
        bbox bound_box;

    public:    
        sphere(const vec3& cen, float rad, shared_ptr<material> mat) : 
            center(cen, vec3()),
            radius(std::fmax(0, rad)),
            mat(mat),
            bound_box(cen - rad, cen + rad) {}

        sphere(const vec3& cen1, const vec3& cen2, float rad, shared_ptr<material> mat) : 
            center(cen1, cen2 - cen1),
            radius(std::fmax(0, rad)),
            mat(mat),
            bound_box(bbox(cen1 - rad, cen1 + rad), bbox(cen2 - rad, cen2 + rad)) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 current_center = center.at(r.time());
            vec3 oc = current_center - r.pt();
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
            rec.normal = (rec.pt - current_center) / radius;
            rec.mat = mat;

            return true;
        }

        bbox bounding_box() const override { return bound_box; }
};

#endif