#ifndef BBOX_H
#define BBOX_H

#include "ray.h"

class bbox {
    private:
        union { 
            interval intervals[3];
            struct {interval x, y, z;};
        };
        float barea;
        float bvolume;
        bool dirty = true;

        void recompute() {
            barea = 2 * (x.size() * y.size() +
                         x.size() * z.size() + 
                         y.size() * z.size());
            bvolume = x.size() * y.size() * z.size();
            dirty = false;
        }

        void pad() {
            float delta = 0.0001;

            if (x.size() < delta) x.max += delta;
            if (y.size() < delta) y.max += delta;
            if (z.size() < delta) z.max += delta;
        }

    public:
        bbox() : bbox(vec3(), vec3()) {}
        bbox(vec3 min, vec3 max) {
            for (int axis = 0; axis < 3; ++axis) {
                intervals[axis] = (max[axis] > min[axis]) ? interval(min[axis], max[axis]) : interval(max[axis], min[axis]);
            }

            pad();
        }

        bbox(const interval& x, const interval& y, const interval& z) :
            x(x), y(y), z(z) { pad(); }

        bbox(const bbox& a, const bbox& b) : 
            x(a.x, b.x),
            y(a.y, b.y),
            z(a.z, b.z) {}

        bbox(const bbox& box) :
            x(box.x),
            y(box.y),
            z(box.z) {}

        interval& operator[](int i) { return intervals[i]; };

        bool intersects(const vec3& point) const {
            return x.contains(point.x) && y.contains(point.y) && z.contains(point.z);
        }

        double area() {
            if (dirty) recompute();
            return barea;
        }

        float left_area(int axis, float position) {
            interval prime(intervals[axis].min, position);
            return 2 * (prime.size() * intervals[(axis + 1) % 3].size() +
                        prime.size() * intervals[(axis + 2) % 3].size() + 
                        intervals[(axis + 1) % 3].size() * intervals[(axis + 2) % 3].size());
        }

        float right_area(int axis, float position) {
            interval prime(position, intervals[axis].max);
            return 2 * (prime.size() * intervals[(axis + 1) % 3].size() +
                        prime.size() * intervals[(axis + 2) % 3].size() + 
                        intervals[(axis + 1) % 3].size() * intervals[(axis + 2) % 3].size());
        }

        double volume() {
            if (dirty) recompute();
            return bvolume;
        }

        bool hit(const ray& r, interval ray_t) const {
            const vec3& point = r.pt();
            const vec3& dir = r.dir();

            for (int axis = 0; axis < 3; ++axis) {
                const interval& ax = intervals[axis];
                float t0 = (ax.min - point[axis]) / dir[axis];
                float t1 = (ax.max - point[axis]) / dir[axis];
            

                if (t0 < t1) {
                    ray_t.min = std::max(t0, ray_t.min);
                    ray_t.max = std::min(t1, ray_t.max);
                } else {
                    ray_t.min = std::max(t1, ray_t.min);
                    ray_t.max = std::min(t0, ray_t.max);
                }

                if (ray_t.max <= ray_t.min) return false;
            }
            return true;
        }
};

#endif