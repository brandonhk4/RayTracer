#ifndef CAMERA_H
#define CAMERA_H

// #ifndef STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #include "external/stb_image.h"
// #endif

// #ifndef STB_IMAGE_WRITE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#ifndef STBI_WRITE_NO_STDIO
#include "external/stb_image_write.h"
#endif
// #endif

#include "hittable.h"
#include "material.h"

#include <sstream>
#include <string>

using namespace std;

struct config {
    // Screen config
    double aspect_ratio = 16.0 / 9.0;   // Ratio of image width over height
    int image_width = 512;              // Rendered image width in pixel count

    // Render config
    int aa_samples = 10;                // Count of random samples for each pixel for antialiasing
    int max_depth = 8;                 // Maximum number of ray bounce recursions
    
    // Camera config
    double vfov = 90;                   // Vertical view angle (field of view)
    vec3 pos;                           // Point camera is at
    vec3 target = vec3(0, 0, -1);       // Point camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Camera "up" direction. Change to roll camera.

    // Lens config
    double defocus_angle = 0;           // Variation angle of rays through each pixel
    double focus_dist = 0;              // Distance from camera lens to plane of perfect focus
};

class camera {
    private:
        int image_height;           // Rendered image height
        vec3 pixel_center00;        // Location of pixel (0,0)
        vec3 pixel_delta_u;         // Horizontal offset of pixel
        vec3 pixel_delta_v;         // Vertical offset of pixel
        vec3 forward, right, up;    // Camera frame basis vectors
        vec3 defocus_disk_u;        // Horizontal disk radius
        vec3 defocus_disk_v;        // Vertial disk radius
        
        void initialize() {
            image_height = max(int(image_width / aspect_ratio), 1);

            forward = (target - pos).dir();
            right = cross(forward, vup.dir()).dir();
            up = cross(right, forward);

            // Viewport dimensions
            if (focus_dist == 0) focus_dist = (target - pos).length();
            double h = tan(degrees_to_radians(vfov) / 2);
            double viewport_height = 2.0 * h * focus_dist;
            double viewport_width = viewport_height * (double(image_width) / image_height);

            vec3 viewport_u = viewport_width * right;
            vec3 viewport_v = viewport_height * -up;
            
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            vec3 viewport_upper_left = pos + focus_dist * forward -
                                    viewport_u / 2 - viewport_v / 2;
            pixel_center00 = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle) / 2);
            defocus_disk_u = defocus_radius * right;
            defocus_disk_v = defocus_radius * up;
        }

        vec3 defocus_disk_sample() const {
            vec3 p = random_in_unit_disk();
            return pos + p.x * defocus_disk_u + p.y * defocus_disk_v;
        }

        ray get_ray(int i, int j) const{
            vec3 offset = vec3(random_double() - 0.5, random_double() - 0.5, 0);
            vec3 pixel_sample = pixel_center00 + 
                                ((i + offset.x) * pixel_delta_u) + 
                                ((j + offset.y) * pixel_delta_v);

            vec3 ray_pos = (defocus_angle <= 0) ? pos : defocus_disk_sample();

            vec3 ray_dir = pixel_sample - ray_pos;

            return ray(ray_pos, ray_dir);
        }

        vec3 ray_color(const ray& r, int depth, const hittable& world) const {
            if (!depth) return vec3();

            hit_record rec;

            if (world.hit(r, interval(0.001, infinity), rec)) {
                ray scattered;
                vec3 attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered))
                    return attenuation * ray_color(scattered, depth - 1, world);
            }

            vec3 unit_dir = r.dir().dir();
            double a = 0.5 * (unit_dir.y + 1.0);
            return (1.0 - a) * vec3(1.0) + a * vec3(0.5, 0.8, 0.8);
        }

    public:
        // Screen config
        double aspect_ratio;                // Ratio of image width over height
        int image_width;                    // Rendered image width in pixel count

        // Render config
        int aa_samples;                     // Count of random samples for each pixel for antialiasing
        int max_depth;                      // Maximum number of ray bounce recursions
        
        // Camera config
        double vfov;                        // Vertical view angle (field of view)
        vec3 pos;                           // Point camera is at
        vec3 target;                        // Point camera is looking at
        vec3 vup;                           // Camera "up" direction. Change to roll camera.

        // Lens config
        double defocus_angle;               // Variation angle of rays through each pixel
        double focus_dist;                  // Distance from camera lens to plane of perfect focus

        camera(struct config cf) : 
            aspect_ratio(cf.aspect_ratio),
            image_width(cf.image_width),
            aa_samples(cf.aa_samples),
            max_depth(cf.max_depth),
            vfov(cf.vfov),
            pos(cf.pos),
            target(cf.target),
            vup(cf.vup),
            defocus_angle(cf.defocus_angle),
            focus_dist(cf.focus_dist)
        {}

        void render(const hittable& world, string output_file = "image.png") {
            initialize();
            ostringstream render_stream;

            for (int j = 0; j < image_height; j++) {
                clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;
                for (int i = 0; i < image_width; i++) {
                    vec3 pixel_color;
                    for (int sample = 0; sample < aa_samples; ++sample) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world) / aa_samples;
                    }
                    
                    write_color(render_stream, pixel_color);
                }
            }
            clog << "\rDone.                 \n";

            int success = stbi_write_png(output_file.c_str(), image_width, image_height, 3, render_stream.str().c_str(), image_width * 3);
            if (success) cout << "Successfully created image.png\n";
            else cout << "Failed to write image\n";
        }
};

#endif