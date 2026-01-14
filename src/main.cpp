#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <sstream>

using namespace std;

vec3 ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        cout << "hit\n";
        return 0.5 * vec3(rec.normal + vec3(1.0));
    }

    vec3 unit_dir = r.direction().dir();
    double a = 0.5 * (unit_dir.y + 1.0);
    return (1.0 - a) * vec3(1.0) + a * vec3(0.5, 0.8, 0.8);
}

int main() {

    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 512;
    int image_height = max(int(image_width / aspect_ratio), 1);

    // World
    hittable_list world;
    world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100));

    // Camera
    double viewport_height = 2.0;
    double viewport_width = viewport_height * double(image_width) / image_height;
    double focal_length = 1.0;
    vec3 camera_center;

    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) -
                               viewport_u / 2 - viewport_v / 2;
    vec3 pixel_center00 = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    ostringstream render;

    for (int j = 0; j < image_height; j++) {
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;
        for (int i = 0; i < image_width; i++) {
            vec3 pixel_center = pixel_center00 + (i * pixel_delta_u) + (j * pixel_delta_v);
            vec3 ray_dir = pixel_center - camera_center;
            ray r = ray(camera_center, ray_dir);

            vec3 pixel_color = ray_color(r, world);
            write_color(render, pixel_color);
        }
    }
    clog << "\rDone.                 \n";

    int success = stbi_write_png("image.png", image_width, image_height, 3, render.str().c_str(), image_width * 3);
    if (success) cout << "Successfully created image.png\n";
    else cout << "Failed to write image\n";
}