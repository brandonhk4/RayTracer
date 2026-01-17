#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"

using namespace std;

int main() {

    // World
    hittable_list world;

    shared_ptr<lambertian> ground_mat = make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, ground_mat));

    for (int a = -10; a < 10; ++a) {
        for (int b = -10; b < 10; ++b) {
            double choose_mat = random_double();
            vec3 center = vec3(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_mat;

                if (choose_mat < 0.8) {
                    // diffuse
                    vec3 albedo = vec3::random();
                    sphere_mat = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_mat));
                } else if (choose_mat < 0.95) {
                    // reflective
                    vec3 albedo = vec3::random();
                    double fuzz = random_double(0, 0.5);
                    sphere_mat = make_shared<fuzzy>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_mat));
                } else {
                    // glass
                    vec3 albedo = vec3::random(0.8, 1.0);
                    sphere_mat = make_shared<dielectric>(albedo, 1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_mat));
                }
            }
        }
    }

    shared_ptr<lambertian> matte_mat = make_shared<lambertian>(vec3(0.4, 0.6, 0.4));
    world.add(make_shared<sphere>(vec3(-4, 1, 0), 1, matte_mat));
    
    shared_ptr<reflective> reflect_mat = make_shared<reflective>(vec3(0.3, 0.3, 0.3));
    world.add(make_shared<sphere>(vec3(0, 1, 0), 1, reflect_mat));

    shared_ptr<dielectric> di_mat = make_shared<dielectric>(vec3(1.0), 1.5);
    shared_ptr<dielectric> bubble_mat = make_shared<dielectric>(vec3(1.0), 1.0 / 1.5);
    world.add(make_shared<sphere>(vec3(4, 1, 0), 1, di_mat));
    world.add(make_shared<sphere>(vec3(4, 1, 0), .9, bubble_mat));
    

    camera cam;
    // Render config
    cam.aa_samples = 50;
    cam.max_depth = 16;
    
    // Camera config
    cam.vfov = 20;
    cam.pos = vec3(13, 2, 3);
    cam.lookat = vec3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    // Lens config
    cam.defocus_angle = .6;
    cam.focus_dist = 10.0;

    cam.render(world);
}