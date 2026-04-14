#ifndef PATCH_H
#define PATCH_H

#include "hittable.h"

class patch : public hittable{
    private:
        union {
            vec3 pts[4];
            struct {
                vec3 p0, p1, p2, p3;
            };
        };

        shared_ptr<material> mat;
        bbox bound_box;

        float UofV(float v, float A1, float B1, float C1, float D1, float A2, float B2, float C2, float D2) const {
            float a = v * A2 + B2;
            float b = v * (A2 - A1) + B2 - B1;
            if (std::fabs(a) < std::fabs(b)) {
                return (-v * C2 - D2) / a;
            }
            return (v * (C1 - C2) + D1 - D2) / b;
        }

        void interpolate(float u, float v, vec3& point, vec3& normal) const {
            vec4 w((1.0f - u) * (1.0f - v), (1.0f - u) * v, u * (1.0f - v), u * v);
            point = w.x * p0 + w.y * p1 + w.z * p2 + w.w * p3;

            vec4 wdu(v - 1.0f, -v, 1.0f - v, v);
            vec4 wdv(u - 1.0f, 1.0f - u, -u, u);
            vec3 du = wdu.x * p0 + wdu.y * p1 + wdu.z * p2 + wdu.w * p3;
            vec3 dv = wdv.x * p0 + wdv.y * p1 + wdv.z * p2 + wdv.w * p3;
            normal = cross(du, dv).dir();
        }

        float get_t(const vec3& pt, const vec3& o, const vec3& q) const {
            if (std::abs(q.x) >= std::abs(q.y) && std::abs(q.x) >= std::abs(q.z))
                return (pt.x - o.x) / q.x;
            if (std::abs(q.y) >= std::abs(q.z))
                return (pt.y - o.y) / q.y;
            return (pt.z - o.z) / q.z;
        }

        bool solve(const vec3& o, const vec3& q, float v, 
                    float A1, float B1, float C1, float D1, 
                    float A2, float B2, float C2, float D2, 
                    const interval& ray_t, hit_record& rec) const {
            static interval zero_one(0.0f, 1.0f);
            if (!zero_one.surrounds(v)) return false;
            float u = UofV(v, A1, B1, C1, D1, A2, B2, C2, D2);
            if (!zero_one.surrounds(u)) return false;
            vec3 pt, n;
            interpolate(u, v, pt, n);
            float t = get_t(pt, o, q);
            if (!ray_t.surrounds(t)) return false;
            rec.pt = pt;
            rec.normal = n;
            rec.t = t;
            rec.u = u;
            rec.v = v;
            rec.mat = mat;
            return true;
        }

    public:
        patch(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3,
              shared_ptr<material> mat) :
              p0(p0), p1(p1), p2(p2), p3(p3), mat(mat),
              bound_box(bbox(p0, p1), bbox(p2, p3)) {}
        
        bool hit(const ray& r, interval ray_t, hit_record& rec) const {
                vec3 a = p3 - p2 - p1 + p0;
                vec3 b = p2 - p0;
                vec3 c = p1 - p0;
                vec3 d = p0;

                const vec3& o = r.pt();
                const vec3& q = r.dir();

                float A1 = a.x * q.z - a.z * q.x;
                float B1 = b.x * q.z - b.z * q.x;
                float C1 = c.x * q.z - c.z * q.x;
                float D1 = (d.x - o.x) * q.z - (d.z - o.z) * q.x;
                float A2 = a.y * q.z - a.z * q.y;
                float B2 = b.y * q.z - b.z * q.y;
                float C2 = c.y * q.z - c.z * q.y;
                float D2 = (d.y - o.y) * q.z - (d.z - o.z) * q.y;

                float A = A2 * C1 - A1 * C2;
                float B = A2 * D1 - A1 * D2 + B2 * C1 - B1 * C2;
                float C = B2 * D1 - B1 * D2;

                float disc = B * B - 4.0f * A * C;
                if (disc < 0) return false;
                if (disc == 0) {
                    float v = -B / (2.0f * A);
                    return solve(o, q, v, A1, B1, C1, D1, A2, B2, C2, D2, ray_t, rec);
                }
                float sqrt_disc = std::sqrt(disc);
                hit_record r1, r2;
                float v1 = (-B - sqrt(disc)) / (2.0f * A);
                float v2 = (-B + sqrt(disc)) / (2.0f * A);
                bool b1 = solve(o, q, v1, A1, B1, C1, D1, A2, B2, C2, D2, ray_t, r1);
                if (!b1) {
                    return solve(o, q, v2, A1, B1, C1, D1, A2, B2, C2, D2, ray_t, rec);
                } else {
                    bool b2 = solve(o, q, v2, A1, B1, C1, D1, A2, B2, C2, D2, ray_t, r2);
                    if (!b2) {
                        rec = r1;
                        return true;
                    }
                    if (r1.t < r2.t) {
                        rec = r1;
                        return true;
                    }
                    rec = r2;
                    return true;
                }
        }

        bbox bounding_box() const { return bound_box; }
        
};

#endif