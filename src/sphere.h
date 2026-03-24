#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "onb.h"
#include "raytracer.h"

class sphere : public hittable {
    private:
        const ray center;    
        const float radius;
        shared_ptr<material> mat;
        bbox bound_box;

        static void get_sphere_uv(const vec3& p, float& u, float& v) {
            // p: given point on a unit sphere
            // u: returned value [0, 1] of angle wrapping around y-axis.
            // v: returned value [0, 1] of angle from south to north pole.

            float theta = std::acos(-p.y);
            float phi = std::atan2(-p.z, p.x) + pi;

            u = phi / (2.0f * pi);
            v = theta / pi;
        }

        static vec3 random_to_sphere(float radius, float dist_sq) {
            float r1 = random_float();
            float r2 = random_float();
            float z = 1.0f + r2 * (std::sqrt(1.0f - radius * radius / dist_sq) - 1);

            float phi = 2.0f * pi * r1;
            float sin = std::sqrt(1 - z * z);
            float x = std::cos(phi) * sin;
            float y = std::sin(phi) * sin;

            return vec3(x, y, z);
        } 

    public:    
        sphere(const vec3& cen, float rad, shared_ptr<material> mat) : 
            center(cen, vec3()),
            radius(std::fmax(0.0f, rad)),
            mat(mat),
            bound_box(cen - rad, cen + rad) {}

        sphere(const vec3& cen1, const vec3& cen2, float rad, shared_ptr<material> mat) : 
            center(cen1, cen2 - cen1),
            radius(std::fmax(0.0f, rad)),
            mat(mat),
            bound_box(bbox(cen1 - rad, cen1 + rad), bbox(cen2 - rad, cen2 + rad)) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 current_center = center.at(r.time());
            vec3 oc = current_center - r.pt();
            float a = r.dir().length_squared();
            float h = dot(r.dir(), oc);
            float c = oc.length_squared() - radius * radius;

            float discriminant = h * h - a * c;
            if (discriminant < 0.0f) return false;
            
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
            get_sphere_uv(rec.normal, rec.u, rec.v);

            return true;
        }

        bbox bounding_box() const override { return bound_box; }

        float pdf_value(const vec3& origin, const vec3& direction) const override {
            hit_record rec;
            if (!this->hit(ray(origin, direction), interval(0.001, infinity), rec)) return 0;

            float dist_sq = (center.pt() - origin).length_squared();
            float cos = std::sqrt(1.0f - radius * radius / dist_sq);
            float solid_angle = 2.0f * pi * (1.0f - cos);

            return 1.0f / solid_angle;
        }

        vec3 random(const vec3& origin) const override {
            vec3 direction = center.pt() - origin;
            float dist_sq = direction.length_squared();
            onb uvw(direction);
            return uvw.transform(random_to_sphere(radius, dist_sq));
        }
};

#endif