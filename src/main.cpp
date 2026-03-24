#include <SFML/Graphics.hpp>
// Replace this with imGUI one day

// #include "OpenCLHelper.h"

#include <thread>

#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "material.h"
#include "constant_medium.h"
#include "sphere.h"
#include "quad.h"
#include "triangle.h"
#include "bezier.h"
#include "transform.h"
#include "camera.h"
#include "InputParser.h"

using namespace std;

sf::Image display(vector<uint8_t>& pixels, sf::Vector2u size) {
    sf::RenderWindow window(sf::VideoMode(size), "RayTracer", sf::Style::Default, sf::State::Windowed);
    sf::Texture texture(size);
    sf::Sprite sprite(texture);

    // To prevent double clicks
    sf::Vector2i prevPos;

    window.draw(sprite);
    window.display();
    cout << "Attempting to show window\n";
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (prevPos != mousePos && mousePos.x <= size.x && mousePos.y <= size.y) {
                    int index = (mousePos.x + mousePos.y * size.x) * 4;
                    cout << "Pixel at (" << mousePos.x << ", " << mousePos.y << "): "; 
                    cout << '(' << int(pixels[index]) << ", " << int(pixels[index + 1]) << ", " << int(pixels[index + 2]) << ")\n";
                    prevPos = mousePos;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                window.close();
            }
        }
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return texture.copyToImage();
}

hittable_list bouncing_balls(config& cf) {
    hittable_list world;

    auto checker = make_shared<checker_texture>(.32f, vec3(.1f, .3f, .2f), vec3(.9f, .9f, .9f));
    shared_ptr<lambertian> ground_mat = make_shared<lambertian>(checker);
    world.add(make_shared<sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, ground_mat));

    for (int a = -10; a < 10; ++a) {
        for (int b = -10; b < 10; ++b) {
            double choose_mat = random_float();
            vec3 center = vec3(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

            if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
                shared_ptr<material> sphere_mat;

                if (choose_mat < 0.8f) {
                    // diffuse
                    vec3 albedo = vec3::random();
                    sphere_mat = make_shared<lambertian>(albedo);
                    vec3 center2 = center + vec3(0.0f, random_float(0.0f, .5f), 0.0f);
                    world.add(make_shared<sphere>(center, center2, 0.2f, sphere_mat));
                } else if (choose_mat < 0.95f) {
                    // metal
                    vec3 albedo = vec3::random();
                    double fuzz = random_float(0.0f, 0.5f);
                    sphere_mat = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_mat));
                } else {
                    // glass
                    vec3 albedo = vec3::random(0.8f, 1.0f);
                    sphere_mat = make_shared<dielectric>(albedo, 1.5f);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_mat));
                }
            }
        }
    }

    shared_ptr<lambertian> matte_mat = make_shared<lambertian>(vec3(0.4f, 0.6f, 0.4f));
    world.add(make_shared<sphere>(vec3(-4.0f, 1.0f, 0.0f), 1.0f, matte_mat));
    
    shared_ptr<metal> reflect_mat = make_shared<metal>(vec3(0.3f, 0.3f, 0.3f));
    world.add(make_shared<sphere>(vec3(0.0f, 1.0f, 0.0f), 1.0f, reflect_mat));

    shared_ptr<dielectric> di_mat = make_shared<dielectric>(vec3(1.0f), 1.5f);
    shared_ptr<dielectric> bubble_mat = make_shared<dielectric>(vec3(1.0f), 1.0f / 1.5f);
    world.add(make_shared<sphere>(vec3(4.0f, 1.0f, 0.0f), 1.0f, di_mat));
    world.add(make_shared<sphere>(vec3(4.0f, 1.0f, 0.0f), .9f, bubble_mat));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;
    cf.tw = 256;
    cf.th = 144;

    cf.vfov = 20.0f;
    cf.pos = vec3(13.0f, 2.0f, 3.0f);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6f;
    cf.focus_dist = 10.0f;

    cf.background = vec3(0.5f, 0.7f, 0.8f);

    return world;
}

hittable_list checkered_spheres(config& cf) {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32f, vec3(.2f, .3f, .1f), vec3(.9f, .9f, .9f));

    world.add(make_shared<sphere>(vec3(0.0f,-10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(vec3(0.0f, 10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));
    
    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20.0f;
    cf.pos = vec3(13.0f, 2.0f, 3.0f);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6f;
    cf.focus_dist = 10.0f;

    cf.background = vec3(0.5f, 0.7f, 0.8f);
    
    return world;
}

hittable_list earth(config& cf) {
    hittable_list world;

    auto earth_texture = make_shared<image_texture>("earth.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    world.add(make_shared<sphere>(vec3(), 2.0f, earth_surface));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20.0f;
    cf.pos = vec3(13.0f, 2.0f, 3.0f);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6f;
    cf.focus_dist = 10.0f;

    cf.background = vec3(0.5f, 0.7f, 0.8f);

    return world;
}

hittable_list perlin_spheres(config& cf) {
    hittable_list world;

    auto perlin_texture = make_shared<noise_texture>(.5f, 4);
    auto perlin_mat = make_shared<lambertian>(perlin_texture);
    world.add(make_shared<sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, perlin_mat));
    world.add(make_shared<sphere>(vec3(0.0f, 2.0f, 0.0f), 2.0f, perlin_mat));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20.0f;
    cf.pos = vec3(13.0f, 2.0f, 3.0f);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6f;
    cf.focus_dist = 10.0f;

    cf.background = vec3(0.5f, 0.7f, 0.8f);

    return world;
}

hittable_list quads(config& cf) {
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(vec3(1.0f, 0.2f, 0.2f));
    auto back_green   = make_shared<lambertian>(vec3(0.2f, 1.0f, 0.2f));
    auto right_blue   = make_shared<lambertian>(vec3(0.2f, 0.2f, 1.0f));
    auto upper_orange = make_shared<lambertian>(vec3(1.0f, 0.5f, 0.0f));
    auto lower_teal   = make_shared<lambertian>(vec3(0.2f, 0.8f, 0.8f));

    // Quads
    world.add(make_shared<quad>(vec3(-3.0f,-2.0f, 5.0f), vec3(0.0f, 0.0f,-4.0f), vec3(0.0f, 4.0f, 0.0f), left_red));
    world.add(make_shared<quad>(vec3(-2.0f,-2.0f, 0.0f), vec3(4.0f, 0.0f, 0.0f), vec3(0.0f, 4.0f, 0.0f), back_green));
    world.add(make_shared<quad>(vec3( 3.0f,-2.0f, 1.0f), vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 4.0f, 0.0f), right_blue));
    world.add(make_shared<quad>(vec3(-2.0f, 3.0f, 1.0f), vec3(4.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 4.0f), upper_orange));
    world.add(make_shared<quad>(vec3(-2.0f,-3.0f, 5.0f), vec3(4.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f,-4.0f), lower_teal));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 80.0f;
    cf.pos = vec3(0.0f, 0.0f, 9.0f);
    cf.target = vec3();

    cf.defocus_angle = 0.0f;

    cf.background = vec3(0.5f, 0.7f, 0.8f);

    return world;
}

pair<hittable_list, hittable_list> simple_light(config& cf) {
    hittable_list world;
    hittable_list lights;

    auto perlin_texture = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(perlin_texture)));
    world.add(make_shared<sphere>(vec3(0.0f, 2.0f, 0.0f), 2.0f, make_shared<lambertian>(perlin_texture)));

    auto light = make_shared<emissive>(vec3(4.0f));
    lights.add(make_shared<sphere>(vec3(0.0f, 7.0f, 0.0f), 2.0f, light));
    lights.add(make_shared<quad>(vec3(3.0f, 1.0f, -2.0f), vec3(2.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f), light));

    world.add(lights);

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20.0f;
    cf.pos = vec3(26.0f, 3.0f, 6.0f);
    cf.target = vec3(0.0f, 2.0f, 0.0f);

    cf.defocus_angle = 0.0f;

    return pair<hittable_list, hittable_list>(world, lights);
}

pair<hittable_list, hittable_list> cornell_box(config& cf) {
    hittable_list world;
    hittable_list lights;

    auto red   = make_shared<lambertian>(vec3(.65f, .05f, .05f));
    auto white = make_shared<lambertian>(vec3(.73f, .73f, .73f));
    auto green = make_shared<lambertian>(vec3(.12f, .45f, .15f));
    auto light = make_shared<emissive>(vec3(15.0f));

    // walls
    world.add(make_shared<quad>(vec3(555.0f,   0.0f,   0.0f), vec3(   0.0f,   0.0f,  555.0f), vec3(  0.0f, 555.0f,    0.0f), green));
    world.add(make_shared<quad>(vec3(  0.0f,   0.0f,   0.0f), vec3(   0.0f, 555.0f,    0.0f), vec3(  0.0f,   0.0f,  555.0f), red));
    world.add(make_shared<quad>(vec3(  0.0f,   0.0f,   0.0f), vec3(   0.0f,   0.0f,  555.0f), vec3(555.0f,   0.0f,    0.0f), white));
    world.add(make_shared<quad>(vec3(555.0f, 555.0f, 555.0f), vec3(-555.0f,   0.0f,    0.0f), vec3(  0.0f,   0.0f, -555.0f), white));
    world.add(make_shared<quad>(vec3(  0.0f,   0.0f, 555.0f), vec3(   0.0f, 555.0f,    0.0f), vec3(555.0f,   0.0f,    0.0f), white));

    // light
    lights.add(make_shared<quad>(vec3(343.0f, 554.0f, 332.0f), vec3(-130.0f,   0.0f, 0.0f), vec3(0.0f,   0.0f, -105.0f), light));

    // objects
    auto aluminum = make_shared<metal>(vec3(0.8f, 0.85f, 0.88f));
    auto box1 = make_shared<transform_o>(box(vec3(), vec3(165.0f, 330.0f, 165.0f), aluminum));
    box1->rotate(15.0f, transform_o::ROTATE_Y);
    box1->translate(vec3(265.0f, 0.0f, 295.0f));
    world.add(box1);

    auto glass = make_shared<dielectric>(1.5f); 
    lights.add(make_shared<sphere>(vec3(190.0f, 255.0f, 190.f), 90.0f, glass));
    auto box2 = make_shared<transform_o>(box(vec3(), vec3(165.0f), white));
    box2->rotate(-18.0f, transform_o::ROTATE_Y);
    box2->translate(vec3(130.0f, 0.0f, 65.0f));
    world.add(box2);
    world.add(lights);

    cf.aspect_ratio = 1.0f;
    cf.image_width  = 600;
    cf.tw           = 200;
    cf.th           = 200;
    cf.aa_samples   = 200;
    cf.max_depth    = 50;

    cf.vfov   = 40.0f;
    cf.pos    = vec3(278.0f, 278.0f, -800.0f);
    cf.target = vec3(278.0f, 278.0f, 0.0f);

    cf.defocus_angle = 0.0f;

    return pair<hittable_list, hittable_list>(world, lights);
}

hittable_list cornell_smoke(config& cf) {
    hittable_list world;

    auto red   = make_shared<lambertian>(vec3(.65f, .05f, .05f));
    auto white = make_shared<lambertian>(vec3(.73f, .73f, .73f));
    auto green = make_shared<lambertian>(vec3(.12f, .45f, .15f));
    auto light = make_shared<emissive>(vec3(15.0f, 15.0f, 15.0f));

    world.add(make_shared<quad>(vec3(555.0f,   0.0f,   0.0f), vec3(   0.0f,   0.0f,  555.0f), vec3(  0.0f, 555.0f,    0.0f), green));
    world.add(make_shared<quad>(vec3(  0.0f,   0.0f,   0.0f), vec3(   0.0f, 555.0f,    0.0f), vec3(  0.0f,   0.0f,  555.0f), red));
    world.add(make_shared<quad>(vec3(  0.0f,   0.0f,   0.0f), vec3(   0.0f,   0.0f,  555.0f), vec3(555.0f,   0.0f,    0.0f), white));
    world.add(make_shared<quad>(vec3(555.0f, 555.0f, 555.0f), vec3(-555.0f,   0.0f,    0.0f), vec3(  0.0f,   0.0f, -555.0f), white));
    world.add(make_shared<quad>(vec3(  0.0f,   0.0f, 555.0f), vec3(   0.0f, 555.0f,    0.0f), vec3(555.0f,   0.0f,    0.0f), white));

    world.add(make_shared<quad>(vec3(343.0f, 554.0f, 332.0f), vec3(-130.0f,   0.0f, 0.0f), vec3(0.0f,   0.0f, -105.0f), light));

    auto box1 = make_shared<transform_o>(box(vec3(), vec3(165.0f, 330.0f, 165.0f), white));
    box1->rotate(15.0f, transform_o::ROTATE_Y);
    box1->translate(vec3(265.0f, 0.0f, 295.0f));
    
    auto box2 = make_shared<transform_o>(box(vec3(), vec3(165.0f), white));
    box2->rotate(-18.0f, transform_o::ROTATE_Y);
    box2->translate(vec3(130.0f, 0.0f, 65.0f));

    world.add(make_shared<constant_medium>(box1, 0.01f, vec3()));
    world.add(make_shared<constant_medium>(box2, 0.01f, vec3(1.0f)));

    cf.aspect_ratio = 1.0f;
    cf.image_width  = 600;
    cf.tw           = 200;
    cf.th           = 200;
    cf.aa_samples   = 200;
    cf.max_depth    = 50;

    cf.vfov   = 40.0f;
    cf.pos    = vec3(278.0f, 278.0f, -800.0f);
    cf.target = vec3(278.0f, 278.0f, 0.0f);

    cf.defocus_angle = 0.0f;

    return world;
}

pair<hittable_list, hittable_list> test(config& cf) {
    hittable_list world;
    hittable_list lights;

    auto reflect = make_shared<metal>(vec3(1.0f));
    auto refract = make_shared<dielectric>(1.5f);
    auto white = make_shared<lambertian>(vec3(.73f));

    auto perlin_texture = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(perlin_texture)));
    // world.add(make_shared<sphere>(vec3(0.0f, 2.0f, 0.0f), 2.0f, reflect));
    world.add(box(vec3(-2.0f, 0.0f, -2.0f), vec3(2.0f, 1.0f, 2.0f), refract));

    auto light = make_shared<emissive>(vec3(4));
    lights.add(make_shared<sphere>(vec3(0.0f, 7.0f, 0.0f), 2.0f, light));
    world.add(lights);
    // auto trans = make_shared<dielectric>(vec3(0.2, 0.6, 0.3), 1.5);
    
    world.add(make_shared<sphere>(vec3(0.0f, 7.0f, 0.0f), 2.0f, light));
    // world.add(make_shared<sphere>(vec3(0, 7, 0), 2.3, trans));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;
    cf.tw = 256;
    cf.th = 144;

    cf.vfov = 20;
    cf.pos = vec3(26, 3, 6);
    cf.target = vec3(0, 2, 0);

    return pair<hittable_list, hittable_list>(world, lights);
}

pair<hittable_list, hittable_list> final_scene(config& cf) {
    hittable_list world;
    hittable_list lights;

    hittable_list boxes1;
    auto ground = make_shared<lambertian>(vec3(0.48f, 0.83f, 0.53f));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0f;
            auto x0 = -1000.0f + i*w;
            auto z0 = -1000.0f + j*w;
            auto y0 = 0.0f;
            auto x1 = x0 + w;
            auto y1 = random_float(1.0f,101.0f);
            auto z1 = z0 + w;

            boxes1.add(box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground));
        }
    }

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<emissive>(vec3(7.0f));
    lights.add(make_shared<quad>(vec3(123.0f, 554.0f, 147.0f), vec3(300.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 265.0f), light));
    world.add(lights);

    auto center1 = vec3(400.0f, 400.0f, 200.0f);
    auto center2 = center1 + vec3(30.0f, 0.0f, 0.0f);
    auto sphere_material = make_shared<lambertian>(vec3(0.7f, 0.3f, 0.1f));
    world.add(make_shared<sphere>(center1, center2, 50.0f, sphere_material));

    world.add(make_shared<sphere>(vec3(260.0f, 150.0f, 45.0f), 50.0f, make_shared<dielectric>(1.5f)));
    world.add(make_shared<sphere>(vec3(0.0f, 150.0f, 145.0f), 50.0f, make_shared<metal>(vec3(0.8f, 0.8f, 0.9f), 1.0f)));

    auto boundary = make_shared<sphere>(vec3(360.0f, 150.0f, 145.0f), 70.0f, make_shared<dielectric>(1.5f));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2f, vec3(0.2f, 0.4f, 0.9f)));
    boundary = make_shared<sphere>(vec3(), 5000.0f, make_shared<dielectric>(1.5f));
    world.add(make_shared<constant_medium>(boundary, .0001f, vec3(1.0f)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("images/earth.jpg"));
    world.add(make_shared<sphere>(vec3(400.0f, 200.0f, 400.0f), 100.0f, emat));
    auto pertext = make_shared<noise_texture>(0.2f);
    world.add(make_shared<sphere>(vec3(220.0f, 280.0f, 300.0f), 80.0f, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(vec3(.73f));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(vec3::random(0.0f, 165.0f), 10.0f, white));
    }

    world.add(make_shared<transform_o>(
        make_shared<bvh_node>(boxes2))
            ->rotate(15, transform_o::ROTATE_Y)
            ->translate(vec3(-100.0f, 270.0f, 395.0f))
    );

    cf.aspect_ratio      = 1.0;
    cf.image_width       = 400;
    cf.tw                = 200;
    cf.th                = 200;
    cf.aa_samples        = 250;
    cf.max_depth         = 4;

    cf.vfov     = 40.0f;
    cf.pos      = vec3(478.0f, 278.0f, -600.0f);
    cf.target   = vec3(278.0f, 278.0f, 0.0f);
    cf.vup      = vec3(0.0f, 1.0f, 0.0f);

    return pair<hittable_list, hittable_list>(world, lights);
}

pair<hittable_list, hittable_list> bezier(config& cf) {
    hittable_list world;
    hittable_list lights;

    mat4 x, y, z;
    for (int i = 0; i < 16; ++i) {
        x.e[i] = (i % 4) * 4 - 2;
        y.e[i] = sin(i) * 4;
        z.e[i] = (i / 4) * 4 - 2;
    }
    
    bezier_patch bp(x, y, z);

    auto white = make_shared<lambertian>(vec3(.73f));
    world.add(bp.tesselate(18, white));

    auto light = make_shared<emissive>(vec3(4));
    
    lights.add(make_shared<sphere>(vec3(0.0f, 7.0f, 0.0f), 2.0f, light));
    world.add(lights);
    

    cf.image_width = 1024;
    cf.aa_samples  = 50;
    cf.max_depth   = 16;
    cf.tw          = 256;
    cf.th          = 144;

    cf.vfov     = 20.0f;
    cf.pos      = vec3(26.0f, 3.0f, 6.0f);
    cf.target   = vec3(0.0f, 2.0f, 0.0f);

    return pair<hittable_list, hittable_list>(world, lights);
}

int main(int argc, char** argv) {

    InputParser input(argc, argv);
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help") || !input.valid()) {
        InputParser::helpMessage();
        return -1;
    }

    string output_file = input.getCmdOption("--out");
    bool save = !output_file.empty();
    if (save) cout << "Saving to " << output_file << '\n';

    bool window_display = input.cmdOptionExists("--display");
    if (window_display) cout << "Showing display\n";

    bool tree = input.cmdOptionExists("--bvh");

    int scene = 0;
    string scene_str = input.getCmdOption("--scene");
    if (!scene_str.empty()) scene = stoi(scene_str);

    config cf;

    hittable_list world;
    hittable_list lights;
    pair<hittable_list, hittable_list> out;
    switch (scene) {
        default:
            out = test(cf);
            world = out.first;
            lights = out.second;
            break;
        case 1:
            world = bouncing_balls(cf);
            break;
        case 2:
            world = checkered_spheres(cf);
            break;
        case 3:
            world = earth(cf);
            break;
        case 4:
            world = perlin_spheres(cf);
            break;
        case 5:
            world = quads(cf);
            break;
        case 6:
            out = simple_light(cf);
            world = out.first;
            lights = out.second;
            break;
        case 7:
            out = cornell_box(cf);
            world = out.first;
            lights = out.second;
            break;
        case 8:
            world = cornell_smoke(cf);
            break;
        case 9:
            out = final_scene(cf);
            world = out.first;
            lights = out.second;
            break;
        case 10:
            out = bezier(cf);
            world = out.first;
            lights = out.second;
            break;
    }

    configure(input, cf);

    camera cam(cf);
    if (tree) world = hittable_list(make_shared<bvh_node>(world));

    vector<uint8_t> pixels(cam.width() * cam.height() * 4);
    vector<thread> threads;
    threads.reserve(cam.width() * cam.height() / (cf.tw * cf.th));

    if (window_display) {
        thread render(&camera::render, &cam, ref(world), ref(lights), ref(pixels), ref(threads));

        sf::Image image(display(pixels, { (unsigned int)cam.width(), (unsigned int)cam.height() }));

        for (thread& t : threads) if (t.joinable()) t.join();

        if (save) {
            bool success = image.saveToFile(output_file);
            if (success) cout << "Successfully created " << output_file << '\n';
            else cout << "Failed to write image\n";
        }
    } else {
        cam.render(world, lights, pixels, threads);
        for (thread& t : threads) if (t.joinable()) t.join();

        if (save) {
            sf::Image image({ (unsigned int)cam.width(), (unsigned int)cam.height() }, pixels.data());
            bool success = image.saveToFile(output_file);
            if (success) cout << "Successfully created " << output_file << '\n';
            else cout << "Failed to write image\n";
        }
    }
    return 0;
}