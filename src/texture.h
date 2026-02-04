#ifndef TEXTURE_H
#define TEXTURE_H

#include "raytracer.h" 

class texture {
    public:
        virtual ~texture() = default;

        virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class solid_color : public texture {
    private:
        vec3 albedo;
    
    public:
        solid_color(const vec3& albedo) : albedo(albedo) {}

        solid_color(float red, float green, float blue) : albedo(red, green, blue) {}

        vec3 value(float u, float v, const vec3& p) const override {
            return albedo;
        }
};

class checker_texture : public texture {
    private:
        float inv_scale;
        shared_ptr<texture> even;
        shared_ptr<texture> odd;

    public:
        checker_texture(float scale, shared_ptr<texture> even, shared_ptr<texture> odd) :
            inv_scale(1.0 / scale), even(even), odd(odd) {}

        checker_texture(float scale, const vec3& c1, const vec3 c2) :
            checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

        vec3 value(float u, float v, const vec3& p) const override {
            int xInt = int(std::floor(inv_scale * p.x));
            int yInt = int(std::floor(inv_scale * p.y));
            int zInt = int(std::floor(inv_scale * p.z));

            return (xInt + yInt + zInt) % 2 == 0 ? even->value(u, v, p) : odd->value(u, v, p);
        }        
};

#endif