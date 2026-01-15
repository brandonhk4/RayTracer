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

    shared_ptr<lambertian> ground_mat = make_shared<lambertian>(vec3(0.5, 0.6, 0.4));
    shared_ptr<lambertian> center_mat = make_shared<lambertian>(vec3(0.4, 0.3, 0.4));
    shared_ptr<reflective> reflect_mat = make_shared<reflective>(vec3(0.3, 0.3, 0.3));
    shared_ptr<fuzzy> fuzzy_mat = make_shared<fuzzy>(vec3(0.3, 0.3, 0.3), 0.4);
    shared_ptr<dielectric> di_mat = make_shared<dielectric>(vec3(1.0, 1.0, 1.0), .833);

    world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5, center_mat));
    // world.add(make_shared<sphere>(vec3(0, 0, -1), 0.5, di_mat));
    world.add(make_shared<sphere>(vec3(0, -100.5, -1), 100, ground_mat));
    world.add(make_shared<sphere>(vec3(-1.1, 0, -1), 0.5, reflect_mat));
    world.add(make_shared<sphere>(vec3(1.1, 0, -1), 0.5, di_mat));

    camera cam;
    cam.aa_samples = 20;
    cam.max_depth = 4;

    cam.render(world);
}