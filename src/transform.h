#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "hittable.h"

class translate_o : public hittable {
    private:
        shared_ptr<hittable> object;
        vec3 offset;
        bbox bound_box;

        ray transform_ray(const ray& r) {
            vec3 origin;
            vec3 direction;

            return ray(origin, direction, r.time());
        }
    
    public:
        translate_o(shared_ptr<hittable> object, const vec3& offset) : 
            object(object), offset(offset) 
        {
            bound_box = object->bounding_box() + offset;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            ray offset_r(r.pt() - offset, r.dir(), r.time());

            if (!object->hit(offset_r, ray_t, rec))
                return false;
            
            rec.pt += offset;
            return true;
        }

        bbox bounding_box() const override { return bound_box; }
};

class rotate_x : public hittable {
    private:
        shared_ptr<hittable> object;
        float sin_theta;
        float cos_theta;
        bbox bound_box;

    public:
        rotate_x(shared_ptr<hittable> object, float theta, bool radians=false) : 
            object(object)
        {
            if (!radians) theta = degrees_to_radians(theta);
            sin_theta = std::sin(theta);
            cos_theta = std::cos(theta);
            bound_box = object->bounding_box();

            vec3 min(infinity, infinity, infinity);
            vec3 max(-infinity, -infinity, -infinity);
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    for (int k = 0; k < 2; ++k) {
                        float x = i * bound_box.x.max + (1 - i) * bound_box.x.min;
                        float y = j * bound_box.y.max + (1 - j) * bound_box.y.min;
                        float z = k * bound_box.z.max + (1 - k) * bound_box.z.min;

                        float newy = cos_theta * y - sin_theta * z;
                        float newz = sin_theta * y + cos_theta * z;

                        vec3 test(x, newy, newz);
                        for (int c = 0; c < 3; ++c) {
                            min[c] = std::fmin(min[c], test[c]);
                            max[c] = std::fmax(max[c], test[c]);
                        }
                    }
                }
            }
            bound_box = bbox(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 origin(
                r.pt().x,
                cos_theta * r.pt().y + sin_theta * r.pt().z,
                -sin_theta * r.pt().y + cos_theta * r.pt().z
            );

            vec3 direction(
                r.dir().x,
                cos_theta * r.dir().y + sin_theta * r.dir().z,
                -sin_theta * r.dir().y + cos_theta * r.dir().z
            );

            ray rotated_r(origin, direction, r.time());

            if (!object->hit(rotated_r, ray_t, rec))
                return false;
            
            rec.pt = vec3(
                rec.pt.x,
                cos_theta * rec.pt.y - sin_theta * rec.pt.z,
                sin_theta * rec.pt.y + cos_theta * rec.pt.z
            );

            rec.normal = vec3(
                rec.normal.x,
                cos_theta * rec.normal.y - sin_theta * rec.normal.z,
                sin_theta * rec.normal.y + cos_theta * rec.normal.z
            );

            return true;
        }

        bbox bounding_box() const override { return bound_box; }
};

class rotate_y : public hittable {
    private:
        shared_ptr<hittable> object;
        float sin_theta;
        float cos_theta;
        bbox bound_box;

    public:
        rotate_y(shared_ptr<hittable> object, float theta, bool radians=false) : 
            object(object)
        {
            if (!radians) theta = degrees_to_radians(theta);
            sin_theta = std::sin(theta);
            cos_theta = std::cos(theta);
            bound_box = object->bounding_box();

            vec3 min(infinity, infinity, infinity);
            vec3 max(-infinity, -infinity, -infinity);
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    for (int k = 0; k < 2; ++k) {
                        float x = i * bound_box.x.max + (1 - i) * bound_box.x.min;
                        float y = j * bound_box.y.max + (1 - j) * bound_box.y.min;
                        float z = k * bound_box.z.max + (1 - k) * bound_box.z.min;

                        float newx =  cos_theta * x + sin_theta * z;
                        float newz = -sin_theta * x + cos_theta * z;

                        vec3 test(newx, y, newz);
                        for (int c = 0; c < 3; ++c) {
                            min[c] = std::fmin(min[c], test[c]);
                            max[c] = std::fmax(max[c], test[c]);
                        }
                    }
                }
            }
            bound_box = bbox(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 origin(
                cos_theta * r.pt().x - sin_theta * r.pt().z,
                r.pt().y,
                sin_theta * r.pt().x + cos_theta * r.pt().z
            );

            vec3 direction(
                cos_theta * r.dir().x - sin_theta * r.dir().z,
                r.dir().y,
                sin_theta * r.dir().x + cos_theta * r.dir().z
            );

            ray rotated_r(origin, direction, r.time());

            if (!object->hit(rotated_r, ray_t, rec))
                return false;
            
            rec.pt = vec3(
                cos_theta * rec.pt.x + sin_theta * rec.pt.z,
                rec.pt.y,
                -sin_theta * rec.pt.x + cos_theta * rec.pt.z
            );

            rec.normal = vec3(
                cos_theta * rec.normal.x + sin_theta * rec.normal.z,
                rec.normal.y,
                -sin_theta * rec.normal.x + cos_theta * rec.normal.z
            );

            return true;
        }

        bbox bounding_box() const override { return bound_box; }
};

class rotate_z : public hittable {
    private:
        shared_ptr<hittable> object;
        float sin_theta;
        float cos_theta;
        bbox bound_box;

    public:
        rotate_z(shared_ptr<hittable> object, float theta, bool radians=false) : 
            object(object)
        {
            if (!radians) theta = degrees_to_radians(theta);
            sin_theta = std::sin(theta);
            cos_theta = std::cos(theta);
            bound_box = object->bounding_box();

            vec3 min(infinity, infinity, infinity);
            vec3 max(-infinity, -infinity, -infinity);
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    for (int k = 0; k < 2; ++k) {
                        float x = i * bound_box.x.max + (1 - i) * bound_box.x.min;
                        float y = j * bound_box.y.max + (1 - j) * bound_box.y.min;
                        float z = k * bound_box.z.max + (1 - k) * bound_box.z.min;

                        float newx = cos_theta * x - sin_theta * y;
                        float newy = sin_theta * x + cos_theta * y;

                        vec3 test(newx, newy, z);
                        for (int c = 0; c < 3; ++c) {
                            min[c] = std::fmin(min[c], test[c]);
                            max[c] = std::fmax(max[c], test[c]);
                        }
                    }
                }
            }
            bound_box = bbox(min, max);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 origin(
                cos_theta * r.pt().x + sin_theta * r.pt().y,
                -sin_theta * r.pt().x + cos_theta * r.pt().y,
                r.pt().z
            );

            vec3 direction(
                cos_theta * r.dir().x + sin_theta * r.dir().y,
                -sin_theta * r.dir().x + cos_theta * r.dir().y,
                r.dir().z
            );

            ray rotated_r(origin, direction, r.time());

            if (!object->hit(rotated_r, ray_t, rec))
                return false;
            
            rec.pt = vec3(
                cos_theta * rec.pt.x - sin_theta * rec.pt.y,
                sin_theta * rec.pt.x + cos_theta * rec.pt.y,
                rec.pt.z
            );

            rec.normal = vec3(
                cos_theta * rec.normal.x - sin_theta * rec.normal.y,
                sin_theta * rec.normal.x + cos_theta * rec.normal.y,
                rec.normal.z
            );

            return true;
        }

        bbox bounding_box() const override { return bound_box; }
};

class transform_o : public hittable {
    private:
        shared_ptr<hittable> object;
        bbox bound_box;
    
    public:
        enum type{
            TRANSFORM, ROTATE_X, ROTATE_Y, ROTATE_Z
        };

        transform_o(shared_ptr<hittable> object) : object(object) {
            bound_box = object->bounding_box();
        }

        transform_o(transform_o* o) : object(o->object), bound_box(o->bound_box) {}

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            return object->hit(r, ray_t, rec);
        }

        bbox bounding_box() const override { return bound_box; }

        shared_ptr<transform_o> translate(const vec3& offset) {
            object = make_shared<translate_o>(object, offset);
            bound_box = object->bounding_box();
            return make_shared<transform_o>(this);
        }

        shared_ptr<transform_o> rotate(float theta, type rot_type, bool radians = false) {
            switch (rot_type) {
                case ROTATE_X:
                    object = make_shared<rotate_x>(object, theta, radians);
                    break;
                case ROTATE_Y:
                    object = make_shared<rotate_y>(object, theta, radians);
                    break;
                case ROTATE_Z:
                    object = make_shared<rotate_z>(object, theta, radians);
                    break;
                default:
                    break;
            }
            bound_box = object->bounding_box();
            return make_shared<transform_o>(this);
        }
};

#endif