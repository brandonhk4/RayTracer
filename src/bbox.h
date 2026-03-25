#ifndef BBOX_H
#define BBOX_H

#include "ray.h"

class bbox {
    private:
        float barea;
        float bvolume;
        bool dirty = true;

        void recompute() {
            barea = 2 * (i[0].size() * i[1].size() +
                         i[0].size() * i[2].size() + 
                         i[1].size() * i[2].size());
            bvolume = i[0].size() * i[1].size() * i[2].size();
            dirty = false;
        }

        void pad() {
            float delta = 0.0001f;

            if (i[0].size() < delta) i[0].max += delta;
            if (i[1].size() < delta) i[1].max += delta;
            if (i[2].size() < delta) i[2].max += delta;
        }

    public:
        interval i[3];

        bbox() : bbox(vec3(), vec3()) {}
        bbox(vec3 min, vec3 max) {
            for (int axis = 0; axis < 3; ++axis) {
                i[axis] = (max[axis] > min[axis]) ? interval(min[axis], max[axis]) : interval(max[axis], min[axis]);
            }

            pad();
        }

        bbox(const interval& x, const interval& y, const interval& z) { 
            i[0] = x;
            i[1] = y;
            i[2] = z;
            pad();
        }

        bbox(const bbox& a, const bbox& b) {
            i[0] = interval(a[0], b[0]);
            i[1] = interval(a[1], b[1]);
            i[2] = interval(a[2], b[2]);
        }

        interval operator[](int k) const { return i[k]; }
        interval& operator[](int k) { return i[k]; }

        bool intersects(const vec3& point) const {
            return i[0].contains(point.x) && i[1].contains(point.y) && i[2].contains(point.z);
        }

        double area() {
            if (dirty) recompute();
            return barea;
        }

        float left_area(int axis, float position) {
            interval prime(i[axis].min, position);
            return 2.0f * (prime.size() * i[(axis + 1) % 3].size() +
                           prime.size() * i[(axis + 2) % 3].size() + 
                           i[(axis + 1) % 3].size() * i[(axis + 2) % 3].size());
        }

        float right_area(int axis, float position) {
            interval prime(position, i[axis].max);
            return 2.0f * (prime.size() * i[(axis + 1) % 3].size() +
                           prime.size() * i[(axis + 2) % 3].size() + 
                           i[(axis + 1) % 3].size() * i[(axis + 2) % 3].size());
        }

        double volume() {
            if (dirty) recompute();
            return bvolume;
        }

        bool hit(const ray& r, interval ray_t) const {
            const vec3& point = r.pt();
            const vec3& dir   = r.dir();

            for (int axis = 0; axis < 3; ++axis) {
                const interval& ax = i[axis];
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

bbox operator+(const bbox& box, const vec3& offset) {
    return bbox(box[0] + offset.x, box[1] + offset.y, box[2] + offset.z);
}

bbox operator+(const vec3& offset, const bbox& box) {
    return box + offset;
}

#endif