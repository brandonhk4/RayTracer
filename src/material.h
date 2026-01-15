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
        double fuzz;

    public:
        fuzzy(const vec3& albedo, double fuzz) : reflective(albedo), fuzz(fuzz) {}

        bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override {
            vec3 reflected = reflect(r_in.dir(), rec.normal);
            reflected = reflected.dir() + fuzz * random_unit_vector();
            scattered = ray(rec.pt, reflected);
            attenuation = albedo;
            return dot(scattered.dir(), rec.normal) > 0.0;
        }
};

#endif