#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "texture.h"
#include "../math/pdf.h"

struct scatter_record {
    vec3 attenuation;
    shared_ptr<pdf> pdf_ptr;
    bool skip_pdf;
    ray skip_pdf_ray;
};

class material {
    public:
        virtual ~material() = default;

        virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const {
            return vec3();
        }

        virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {
            return false;
        }

        virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
            return 1.0f;
        }
};

class lambertian : public material {
    private:
        shared_ptr<texture> tex;
    
    public:
        lambertian(const vec3& albedo) : tex(make_shared<solid_color>(albedo)) {}
        lambertian(shared_ptr<texture> tex) : tex(tex) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            srec.attenuation = tex->value(rec.u, rec.v, rec.pt);
            srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
            srec.skip_pdf = false;
            return true;
        }
        
        float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
            float cos_theta = dot(rec.normal, scattered.dir().dir());
            return cos_theta < 0 ? 0 : cos_theta / pi;
        }
};

class metal : public material {
    private:
        vec3 albedo;
        float fuzz;

    public:
        metal(const vec3& albedo, float fuzz = 0.0f) : albedo(albedo), fuzz(fuzz) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            vec3 reflected = reflect(r_in.dir(), rec.normal);
            reflected = reflected.dir() + fuzz * random_unit_vector();

            srec.attenuation = albedo;
            srec.pdf_ptr = nullptr;
            srec.skip_pdf = true;
            srec.skip_pdf_ray = ray(rec.pt, reflected, r_in.time());
            
            return true;
        }
};

class dielectric : public material {
    private:
        vec3 albedo;
        float refract_index;

        static float reflectance(float cos, float refract_index) {
            // Schlick's approximation for reflectance
            float r0 = (1.0f - refract_index) / (1.0f + refract_index);
            r0 *= r0;
            return r0 + (1.0f - r0) * std::pow((1.0f - cos), 5);
        }

    public:
        dielectric(const vec3& albedo, float refract_index) :
                   albedo(albedo), refract_index(refract_index) {}
        
        dielectric(float refract_index) :
                   albedo(1), refract_index(refract_index) {}
        
        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
            srec.attenuation = albedo;
            srec.pdf_ptr = nullptr;
            srec.skip_pdf = true;

            vec3 normal = rec.normal;
            float ri = 1.0f / refract_index;
            if (dot(r_in.dir(), rec.normal) > 0.0f) {
                normal = -normal;
                ri = refract_index;
            }

            vec3 unit_dir = r_in.dir().dir();
            float cos = dot(-unit_dir, normal);
            float sin = std::sqrt(1.0f - cos * cos);

            bool cannot_refract = ri * sin > 1.0;
            vec3 direction;
            vec3 position;
            if (cannot_refract || reflectance(cos, ri) > random_float()) {
                 direction = reflect(unit_dir, normal);
                 position = rec.pt + 0.001f * normal;
            }
            else {
                direction = refract(unit_dir, normal, ri);
                position = rec.pt - 0.001f * normal;
            }
            
            srec.skip_pdf_ray = ray(position, direction, r_in.time());
            return true;
        }
};

class emissive : public material {
    private:
        shared_ptr<texture> tex;

    public:
        emissive(shared_ptr<texture> tex) : tex(tex) {}
        emissive(const vec3& emit) : tex(make_shared<solid_color>(emit)) {}

        vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const override {
            if (dot(r_in.dir(), rec.normal) > -.1) return vec3(0.0f);
            return tex->value(u, v, p);
        }
};

class isotropic : public material {
    private:
        shared_ptr<texture> tex;

    public:
        isotropic(const vec3& albedo) : tex(make_shared<solid_color>(albedo)) {}
        isotropic(shared_ptr<texture> tex) : tex(tex) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override{
            srec.attenuation = tex->value(rec.u, rec.v, rec.pt);
            srec.pdf_ptr = make_shared<sphere_pdf>();
            srec.skip_pdf = false;
            return true;
        }

        float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
            return 1.0f / (4.0f * pi);
        }
};

#endif