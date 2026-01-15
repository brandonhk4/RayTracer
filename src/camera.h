#ifndef CAMERA_H
#define CAMERA_H

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

#include "hittable.h"
#include "material.h"

#include <sstream>

using namespace std;

class camera {
    private:
        int image_height;
        vec3 camera_center;
        vec3 pixel_center00;
        vec3 pixel_delta_u;
        vec3 pixel_delta_v;
        
        void initialize() {
            image_height = max(int(image_width / aspect_ratio), 1);
        
            vec3 camera_center;

            double viewport_height = 2.0;
            double viewport_width = viewport_height * double(image_width) / image_height;
            double focal_length = 1.0;

            vec3 viewport_u = vec3(viewport_width, 0, 0);
            vec3 viewport_v = vec3(0, -viewport_height, 0);
            
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) -
                                    viewport_u / 2 - viewport_v / 2;
            pixel_center00 = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        }

        ray get_ray(int i, int j) const{
            vec3 offset = vec3(random_double() - 0.5, random_double() - 0.5, 0);
            vec3 pixel_sample = pixel_center00 + 
                                ((i + offset.x) * pixel_delta_u) + 
                                ((j + offset.y) * pixel_delta_v);
            vec3 ray_dir = pixel_sample - camera_center;

            return ray(camera_center, ray_dir);
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
        // Image
        double aspect_ratio = 16.0 / 9.0;
        int image_width = 512;
        int aa_samples = 10;
        int max_depth = 15;

        void render(const hittable& world) {
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

            int success = stbi_write_png("image.png", image_width, image_height, 3, render_stream.str().c_str(), image_width * 3);
            if (success) cout << "Successfully created image.png\n";
            else cout << "Failed to write image\n";
        }

};

#endif