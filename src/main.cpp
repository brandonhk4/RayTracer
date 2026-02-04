#include <SFML/Graphics.hpp>
// Replace this with imGUI one day

#include "OpenCLHelper.h"

#include <thread>

#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"
#include "InputParser.h"

using namespace std;

sf::Image display(vector<uint8_t>& pixels, sf::Vector2u size) {
    sf::RenderWindow window(sf::VideoMode(size), "RayTracer", sf::Style::Default, sf::State::Windowed);
    sf::Texture texture(size);
    sf::Sprite sprite(texture);

    window.draw(sprite);
    window.display();
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return texture.copyToImage();
}

hittable_list bouncing_balls() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(.32, vec3(.1, .3, .2), vec3(.9, .9, .9));
    shared_ptr<lambertian> ground_mat = make_shared<lambertian>(checker);
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, ground_mat));

    for (int a = -10; a < 10; ++a) {
        for (int b = -10; b < 10; ++b) {
            double choose_mat = random_float();
            vec3 center = vec3(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_mat;

                if (choose_mat < 0.8) {
                    // diffuse
                    vec3 albedo = vec3::random();
                    sphere_mat = make_shared<lambertian>(albedo);
                    vec3 center2 = center + vec3(0, random_float(0, .5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_mat));
                } else if (choose_mat < 0.95) {
                    // reflective
                    vec3 albedo = vec3::random();
                    double fuzz = random_float(0, 0.5);
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

    return world;
}

int main(int argc, char** argv) {
    InputParser input(argc, argv);
    if (input.cmdOptionExists("-h")) {
        InputParser::helpMessage();
        return -1;
    }

    string output_file = input.getCmdOption("--out");
    bool save = !output_file.empty();

    bool tree = input.cmdOptionExists("--bvh");

    config cf = configure(input);

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(13, 2, 3);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6;
    cf.focus_dist = 10.0;

    camera cam(cf);

    
    // World
    hittable_list world = bouncing_balls();
    if (tree) world = hittable_list(make_shared<bvh_node>(world));

    vector<uint8_t> pixels(cam.width() * cam.height() * 4);

    thread render(&camera::render, &cam, std::ref(world), std::ref(pixels));

    sf::Image image(display(pixels, { (unsigned int)cam.width(), (unsigned int)cam.height() }));

    if (save) {
        bool success = image.saveToFile(output_file);
        if (success) cout << "Successfully created image.png\n";
        else cout << "Failed to write image\n";
    }
}