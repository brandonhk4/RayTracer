#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include "pdf.h"

#include <thread>

#include <string>

using namespace std;

struct config {
    // Screen config
    float aspect_ratio = 16.0f / 9.0f;    // Ratio of image width over height
    int image_width = 1024;               // Rendered image width in pixel count
    int tw = -1;
    int th =  -1;

    // Render config
    int aa_samples = 20;                   // Count of random samples for each pixel for antialiasing
    int max_depth = 16;                    // Maximum number of ray bounce recursions
    
    // Camera config
    float vfov = 90.0f;                    // Vertical view angle (field of view)
    vec3 pos;                              // Point camera is at
    vec3 target = vec3(0.0f, 0.0f, -1.0f); // Point camera is looking at
    vec3 vup = vec3(0.0f, 1.0f, 0.0f);     // Camera "up" direction. Change to roll camera.

    // Lens config
    float defocus_angle = 0.0f;            // Variation angle of rays through each pixel
    float focus_dist = 0.0f;               // Distance from camera lens to plane of perfect focus

    vec3 background;
};

class camera {
    private:
        int image_height;           // Rendered image height
        vec3 pixel_center00;        // Location of pixel (0,0)
        vec3 pixel_delta_u;         // Horizontal offset of pixel
        vec3 pixel_delta_v;         // Vertical offset of pixel
        vec3 defocus_disk_u;        // Horizontal disk radius
        vec3 defocus_disk_v;        // Vertial disk radius
        int tw, th;
        
        void initialize() {
            image_height = max(int(image_width / aspect_ratio), 1);
            if (tw == -1) tw = image_width;
            if (th == -1) th = image_height;
            
            vec3 forward, right, up;    // Camera frame basis vectors
            forward = (target - pos).dir();
            right = cross(forward, vup.dir()).dir();
            up = cross(right, forward);

            // Viewport dimensions
            if (focus_dist == 0) focus_dist = (target - pos).length();
            float h = tan(degrees_to_radians(vfov) / 2.0f);
            float viewport_height = 2.0f * h * focus_dist;
            float viewport_width = viewport_height * (float(image_width) / image_height);

            vec3 viewport_u = viewport_width * right;
            vec3 viewport_v = viewport_height * -up;
            
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            vec3 viewport_upper_left = pos + focus_dist * forward -
                                    viewport_u / 2.0f - viewport_v / 2.0f;
            pixel_center00 = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

            float defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle) / 2);
            defocus_disk_u = defocus_radius * right;
            defocus_disk_v = defocus_radius * up;
        }

        vec3 defocus_disk_sample() const {
            vec3 p = random_in_unit_disk();
            return pos + p.x * defocus_disk_u + p.y * defocus_disk_v;
        }

        ray get_ray(int i, int j) const{
            vec3 offset = vec3(random_float() - 0.5f, random_float() - 0.5f, 0.0f);
            vec3 pixel_sample = pixel_center00 + 
                                ((i + offset.x) * pixel_delta_u) + 
                                ((j + offset.y) * pixel_delta_v);

            vec3 ray_pos = (defocus_angle <= 0.0f) ? pos : defocus_disk_sample();

            vec3 ray_dir = pixel_sample - ray_pos;

            float ray_time = random_float();

            return ray(ray_pos, ray_dir, ray_time);
        }

        vec3 ray_color(const ray& r, int depth, const hittable& world, const hittable& lights) const {
            if (!depth) return vec3();

            hit_record rec;

            if (!world.hit(r, interval(0.001f, infinity), rec)) {
                return background;
            }

            scatter_record srec;
            vec3 emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.pt);

            if (!rec.mat->scatter(r, rec, srec))
                return emission;
            
            if (srec.skip_pdf) {
                return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, world, lights);
            }
            
            float w = lights.empty() ? 0.0f : 0.5f;
            mixture_pdf mixed_pdf(make_shared<hittable_pdf>(lights, rec.pt), srec.pdf_ptr, w);
            ray scattered = ray(rec.pt, mixed_pdf.generate(), r.time());
            float pdf_value = mixed_pdf.value(scattered.dir());
            
            float scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
            
            vec3 scatter_color = (srec.attenuation * scattering_pdf * ray_color(scattered, depth - 1, world, lights)) / pdf_value;
            
            return emission + scatter_color;
        }

        void pixel_color(const hittable* world, const hittable* lights, vector<uint8_t>* pixels, int i, int j) {
            for (int _j = j; _j < j + th; ++_j){
                for (int _i = i; _i < i + tw; ++_i){
                    vec3 pixel_color;
                    for (int sample = 0; sample < aa_samples; ++sample) {
                        ray r = get_ray(_i, _j);
                        pixel_color += ray_color(r, max_depth, *world, *lights) / aa_samples;
                    }

                    write_color(*pixels, pixel_color, (_i + _j * image_width) * 4);
                }    
            }
        }

    public:
        // Screen config
        float aspect_ratio;                // Ratio of image width over height
        int image_width;                    // Rendered image width in pixel count

        // Render config
        int aa_samples;                     // Count of random samples for each pixel for antialiasing
        int max_depth;                      // Maximum number of ray bounce recursions
        
        // Camera config
        float vfov;                        // Vertical view angle (field of view)
        vec3 pos;                           // Point camera is at
        vec3 target;                        // Point camera is looking at
        vec3 vup;                           // Camera "up" direction. Change to roll camera.

        // Lens config
        float defocus_angle;               // Variation angle of rays through each pixel
        float focus_dist;                  // Distance from camera lens to plane of perfect focus

        vec3 background;

        camera(struct config cf) : 
            aspect_ratio(cf.aspect_ratio),
            image_width(cf.image_width),
            tw(cf.tw),
            th(cf.th),
            aa_samples(cf.aa_samples),
            max_depth(cf.max_depth),
            vfov(cf.vfov),
            pos(cf.pos),
            target(cf.target),
            vup(cf.vup),
            defocus_angle(cf.defocus_angle),
            focus_dist(cf.focus_dist),
            background(cf.background)
        {initialize();}

        void render(const hittable& world, const hittable& lights, vector<uint8_t>& pixels, vector<thread>& threads) {
            for (int j = 0; j < image_height; j+=th) {
                clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;
                for (int i = 0; i < image_width; i+=tw) {
                    threads.emplace_back(&camera::pixel_color, this, &world, &lights, &pixels, i, j);
                    // threads[threads.size() - 1].detach();
                }
            }

            clog << "\rDone.                 \n";
        }

        //move this to gpu later
        void generate_rays(float pts[], float dirs[]) {
            for (int j = 0; j < image_height; j++) {
                for (int i = 0; i < image_width; i++) {
                    vec3 pixel_color;
                    for (int sample = 0; sample < aa_samples; ++sample) {
                        ray r = get_ray(i, j);
                        pts[ sample * image_height * image_width * 4 + j * image_width * 4 + i * 4]     = r.pt().x;
                        pts[ sample * image_height * image_width * 4 + j * image_width * 4 + i * 4 + 1] = r.pt().y;
                        pts[ sample * image_height * image_width * 4 + j * image_width * 4 + i * 4 + 2] = r.pt().z;
                        pts[ sample * image_height * image_width * 4 + j * image_width * 4 + i * 4 + 3] = 0;
                        dirs[sample * image_height * image_width * 4 + j * image_width * 4 + i * 4]     = r.dir().x;
                        dirs[sample * image_height * image_width * 4 + j * image_width * 4 + i * 4 + 1] = r.dir().y;
                        dirs[sample * image_height * image_width * 4 + j * image_width * 4 + i * 4 + 2] = r.dir().z;
                        dirs[sample * image_height * image_width * 4 + j * image_width * 4 + i * 4 + 3] = 0;
                    }
                }
            }
        }

        const int width() const { return image_width; }

        const int height() const { return image_height; }
};

#endif