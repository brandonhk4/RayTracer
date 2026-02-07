#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable {
    private:
        vec3 Q;
        vec3 u, v;
        shared_ptr<material> mat;
        bbox bound_box;
        vec3 n;

        static float determinant(vec3 c1, vec3 c2, vec3 c3) {
            vec3 ray_cross = cross(c3, c1);
            return dot(c2, ray_cross);
        }

        void set_bbox() {
            bbox box_diagonal1 = bbox(Q, Q + u + v);
            bbox box_diagonal2 = bbox(Q + u, Q + v);
            bound_box = bbox(box_diagonal1, box_diagonal2);
        }

    public:
        quad(const vec3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) :
            Q(Q), u(u), v(v), mat(mat)
        {
            set_bbox();
            n = cross(u, v).dir();
        }

        bbox bounding_box() const override { return bound_box; }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // using Cramer's rule to determine hits
            float det = determinant(-r.dir(), u, v);
            if (std::fabs(det) < 1e-8) return false;

            vec3 OQ = r.pt() - Q;
            
            float u_det = determinant(-r.dir(), OQ, v);
            float v_det = determinant(-r.dir(), u, OQ);

            float a = u_det / det;
            float b = v_det / det;

            interval i(0, 1);
            if (!i.contains(a) || !i.contains(b)) return false;
            
            float t_det = determinant(OQ, u, v);
            float t = t_det / det;
            if (!ray_t.contains(t)) return false;

            vec3 P = r.at(t);

            rec.t = t;
            rec.pt = P;
            rec.mat = mat;
            rec.normal = n;
            rec.u = a;
            rec.v = b;

            return true;
        }
};

#endif