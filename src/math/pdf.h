#ifndef PDF_H
#define PDF_H

#include "vec3.h"
#include "onb.h"
#include "../objects/hittable.h"

class pdf {
    public:
        virtual ~pdf() {}

        virtual float value(const vec3& direction) const = 0;
        virtual vec3 generate() const = 0;
};

class sphere_pdf : public pdf {
    public:
        sphere_pdf() {}

        float value(const vec3& direction) const override {
            return 1.0f / (4.0f * pi);
        }

        vec3 generate() const override {
            return random_unit_vector();
        }
};

class cosine_pdf : public pdf {
    private:
        onb uvw;

    public:
        cosine_pdf(const vec3& w) : uvw(w) {}

        float value(const vec3& direction) const override {
            float cosine_theta = dot(direction.dir(), uvw.w);
            return std::fmax(0.0f, cosine_theta / pi);
        }

        vec3 generate() const override {
            return uvw.transform(random_cosine_direction());
        }
};

class hittable_pdf : public pdf {
    private:
        const hittable& objects;
        vec3 origin;

    public:
        hittable_pdf(const hittable& objects, const vec3 origin) : objects(objects), origin(origin) {}

        float value(const vec3& direction) const override {
            return objects.pdf_value(origin, direction);
        }

        vec3 generate() const override {
            return objects.random(origin);
        }
};

class mixture_pdf : public pdf {
    private:
        shared_ptr<pdf> p0, p1;
        float w;
    
    public:
        mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1, float w) : 
            p0(p0), p1(p1), w(std::clamp(w, 0.0f, 1.0f)) {}

        float value(const vec3& direction) const override {
            return w * p0->value(direction) + (1 - w) * p1->value(direction);
        }

        vec3 generate() const override {
            if (random_float() < w) return p0->generate();
            else return p1->generate();
        }
};

#endif