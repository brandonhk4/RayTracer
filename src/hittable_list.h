#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "bbox.h"
#include "sphere.h"

#include <vector>

class hittable_list : public hittable {
    private: 
        bbox bound_box;

    public:
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() {objects.clear();}

        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
            bound_box = bbox(bound_box, object->bounding_box());
        }

        void add(hittable_list list) {
            for (const auto& object : list.objects) {
                add(object);
            }
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            float closest = ray_t.max;
            
            for (const auto& object : objects) {
                if (object->hit(r, interval(ray_t.min, closest), temp_rec)) {
                    hit_anything = true;
                    closest = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

        bool empty() const { return objects.empty(); }

        bbox bounding_box() const override { return bound_box; }

        float pdf_value(const vec3& origin, const vec3& direction) const override{
            if (objects.empty()) return 1.0f;
            float pdf_value = 0.0f;
            for (const auto& object : objects) {
                pdf_value += object->pdf_value(origin, direction);
            }
            return pdf_value / objects.size();
        }

        vec3 random(const vec3& origin) const override{
            if (objects.empty()) return random_unit_vector();
            return objects[random_int(0, objects.size())]->random(origin);
        }
};

#endif