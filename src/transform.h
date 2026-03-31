#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "hittable.h"
#include "quat.h"

class transform_o : public hittable {
    private:
        shared_ptr<hittable> object;
        bbox bound_box;
        dquat tf = dquat::eye;
        dquat inv = dquat::eye;
    
    public:
        transform_o(shared_ptr<hittable> object) : object(object) {
            bound_box = object->bounding_box();
        }

        transform_o(transform_o* o) : object(o->object), bound_box(o->bound_box), tf(o->tf), inv(o->inv) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            ray transformed_r = inv.transform(r);

            if (!object->hit(transformed_r, ray_t, rec))
                return false;
            
            rec.pt = tf.transform(rec.pt);
            rec.normal = tf.p.rotate(rec.normal);

            return true;
        }

        bbox bounding_box() const override { return bound_box; }

        shared_ptr<transform_o> translate(const vec3& offset) {
            dquat trans = dquat::translate(offset);
            tf = trans * tf;
            inv = tf.inv();

            bound_box = bound_box + offset;
            return make_shared<transform_o>(this);
        }

        shared_ptr<transform_o> rotate(float theta, const vec3& axis, bool radians = false) {
            if (!radians) theta = degrees_to_radians(theta);

            dquat rot = dquat::rotate(theta, axis);
            tf = rot * tf;
            inv = tf.inv();

            bound_box = object->bounding_box();
            vec3 min(infinity, infinity, infinity);
            vec3 max(-infinity, -infinity, -infinity);
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    for (int k = 0; k < 2; ++k) {
                        float x = i * bound_box[0].max + (1 - i) * bound_box[0].min;
                        float y = j * bound_box[1].max + (1 - j) * bound_box[1].min;
                        float z = k * bound_box[2].max + (1 - k) * bound_box[2].min;

                        vec3 test = tf.transform(vec3(x, y, z));
                        
                        for (int c = 0; c < 3; ++c) {
                            min[c] = std::fmin(min[c], test[c]);
                            max[c] = std::fmax(max[c], test[c]);
                        }
                    }
                }
            }
            bound_box = bbox(min, max);

            return make_shared<transform_o>(this);
        }
};

#endif