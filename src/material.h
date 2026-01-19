#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
    public:
        virtual ~material() = default;

        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
            return false;
        }
};

class lambertian : public material {
    private:
        vec3 albedo;
    
    public:
        lambertian(const vec3& albedo) : albedo(albedo) {}

        bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
            vec3 scatter_dir = rec.normal + random_unit_vector();
            if (scatter_dir.near_zero()) scatter_dir = rec.normal;
            scattered = ray(rec.pt, scatter_dir);
            attenuation = albedo;
            return true;
        }
};

class reflective : public material {
    protected:
        vec3 albedo;

    public:
        reflective(const vec3& albedo) : albedo(albedo) {}

        bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(r_in.dir(), rec.normal);
            scattered = ray(rec.pt, reflected);
            attenuation = albedo;
            return true;
        }
};

class fuzzy : public reflective {
    private:
        float fuzz;

    public:
        fuzzy(const vec3& albedo, float fuzz) : reflective(albedo), fuzz(fuzz) {}

        bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(r_in.dir(), rec.normal);
            reflected = reflected.dir() + fuzz * random_unit_vector();
            scattered = ray(rec.pt, reflected);
            attenuation = albedo;
            return dot(scattered.dir(), rec.normal) > 0.0;
        }
};

class dielectric : public material {
    private:
        vec3 albedo;
        float refract_index;

        static float reflectance(float cos, float refract_index) {
            // Schlick's approximation for reflectance
            float r0 = (1 - refract_index) / (1 + refract_index);
            r0 *= r0;
            return r0 + (1 - r0) * std::pow((1 - cos), 5);
        }

    public:
        dielectric(const vec3& albedo, float refract_index) :
                   albedo(albedo), refract_index(refract_index) {}
        
        bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
            attenuation = albedo;
            vec3 normal = rec.normal;
            float ri = 1.0 / refract_index;
            if (dot(r_in.dir(), rec.normal) > 0.0) {
                normal = -normal;
                ri = refract_index;
            }

            vec3 unit_dir = r_in.dir().dir();
            float cos = dot(-unit_dir, normal);
            float sin = std::sqrt(1.0 - cos * cos);

            bool cannot_refract = ri * sin > 1.0;
            vec3 direction;
            vec3 position;
            if (cannot_refract || reflectance(cos, ri) > random_float()) {
                 direction = reflect(unit_dir, normal);
                 position = rec.pt + 0.001 * normal;
            }
            else {
                direction = refract(unit_dir, normal, ri);
                position = rec.pt - 0.001 * normal;
            }
            
            scattered = ray(position, direction);
            return true;
        }
};

#endif