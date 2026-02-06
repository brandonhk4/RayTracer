#ifndef TEXTURE_H
#define TEXTURE_H

#include "raytracer.h" 
#include "rtw_stb_image.h"
#include "perlin.h"

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

class image_texture : public texture {
    private:
        rtw_image image;
    
    public:
        image_texture(const char* filename) : image(filename) {}

        vec3 value(float u, float v, const vec3& p) const override {
            // If no texture data, return cyan to debug
            if (image.height() <= 0) return vec3(0, 1, 1);

            u = interval(0, 1).clamp(u);
            v = 1.0 - interval(0, 1).clamp(v);

            int i = int(u * image.width());
            int j = int(v * image.height());
            const unsigned char* pixel = image.pixel_data(i, j);

            float color_scale = 1.0f / 255.0f;
            return vec3(pixel[0], pixel[1], pixel[2]) * color_scale;
        }
};

class noise_texture : public texture {
    private:
        perlin noise;
        float scale;
        int turbulence;
    
    public:
        noise_texture(float scale = 1, int turbulence = 1) : scale(scale), turbulence(turbulence) {}
        
        vec3 value(float u, float v, const vec3& p) const override {
            return vec3(1) * noise.turb(p * scale, turbulence);
        }
};

#endif