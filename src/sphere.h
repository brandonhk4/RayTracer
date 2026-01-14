#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "raytracer.h"

class sphere : public hittable {
    private:
        const vec3 center;    
        const double radius;

    public:    
        sphere(const vec3& cen, double rad) : center(cen), radius(std::fmax(0, rad)) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = center - r.point();
            double a = r.direction().length_squared();
            double h = dot(r.direction(), oc);
            double c = oc.length_squared() - radius * radius;

            double discriminant = h * h - a * c;
            if (discriminant < 0) return false;
            
            double sqrtd = std::sqrt(discriminant);
            double root = (h - sqrtd) / a;
            
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            rec.t = root;
            rec.pt = r.at(root);
            rec.normal = (rec.pt - center) / radius;

            return true;
        }
};

#endif