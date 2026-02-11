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

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(13, 2, 3);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6;
    cf.focus_dist = 10.0;

    cf.background = vec3(0.5, 0.7, 0.8);

    return world;
}

hittable_list checkered_spheres(config& cf) {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, vec3(.2, .3, .1), vec3(.9, .9, .9));

    world.add(make_shared<sphere>(vec3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(vec3(0, 10, 0), 10, make_shared<lambertian>(checker)));
    
    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(13, 2, 3);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6;
    cf.focus_dist = 10.0;

    cf.background = vec3(0.5, 0.7, 0.8);
    
    return world;
}

hittable_list earth(config& cf) {
    hittable_list world;

    auto earth_texture = make_shared<image_texture>("earth.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    world.add(make_shared<sphere>(vec3(), 2, earth_surface));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(13, 2, 3);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6;
    cf.focus_dist = 10.0;

    cf.background = vec3(0.5, 0.7, 0.8);

    return world;
}

hittable_list perlin_spheres(config& cf) {
    hittable_list world;

    auto perlin_texture = make_shared<noise_texture>(.5, 4);
    auto perlin_mat = make_shared<lambertian>(perlin_texture);
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, perlin_mat));
    world.add(make_shared<sphere>(vec3(0, 2, 0), 2, perlin_mat));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(13, 2, 3);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6;
    cf.focus_dist = 10.0;

    cf.background = vec3(0.5, 0.7, 0.8);

    return world;
}

hittable_list quads(config& cf) {
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(vec3(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(vec3(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(vec3(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(vec3(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(vec3(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(vec3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(vec3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(vec3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(vec3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(vec3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 80;
    cf.pos = vec3(0, 0, 9);
    cf.target = vec3();

    cf.defocus_angle = 0;

    cf.background = vec3(0.5, 0.7, 0.8);

    return world;
}

hittable_list simple_light(config& cf) {
    hittable_list world;

    auto perlin_texture = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_texture)));
    world.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    auto light = make_shared<emissive>(vec3(4));
    world.add(make_shared<sphere>(vec3(0, 7, 0), 2, light));
    world.add(make_shared<quad>(vec3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), light));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(26, 3, 6);
    cf.target = vec3(0, 2, 0);

    cf.defocus_angle = 0;

    return world;
}

hittable_list cornell_box(config& cf) {
    hittable_list world;

    auto red   = make_shared<lambertian>(vec3(.65, .05, .05));
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    auto green = make_shared<lambertian>(vec3(.12, .45, .15));
    auto light = make_shared<emissive>(vec3(15, 15, 15));

    world.add(make_shared<quad>(vec3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(vec3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(vec3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(vec3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(vec3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(vec3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    auto box1 = make_shared<transform_o>(box(vec3(), vec3(165, 330, 165), white));
    box1->rotate(15, transform_o::ROTATE_Y);
    box1->translate(vec3(265, 0, 295));
    world.add(box1);
    
    auto box2 = make_shared<transform_o>(box(vec3(), vec3(165), white));
    box2->rotate(-18, transform_o::ROTATE_Y);
    box2->translate(vec3(130, 0, 65));
    world.add(box2);

    cf.aspect_ratio = 1.0;
    cf.image_width = 600;
    cf.aa_samples = 200;
    cf.max_depth= 50;

    cf.vfov = 40;
    cf.pos = vec3(278, 278, -800);
    cf.target = vec3(278, 278, 0);
    cf.vup = vec3(0,1,0);

    cf.defocus_angle = 0;

    return world;
}

hittable_list cornell_smoke(config& cf) {
    hittable_list world;

    auto red   = make_shared<lambertian>(vec3(.65, .05, .05));
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    auto green = make_shared<lambertian>(vec3(.12, .45, .15));
    auto light = make_shared<emissive>(vec3(7, 7, 7));

    world.add(make_shared<quad>(vec3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(vec3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(vec3(113,554,127), vec3(330,0,0), vec3(0,0,305), light));
    world.add(make_shared<quad>(vec3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(vec3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(vec3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    auto box1 = make_shared<transform_o>(box(vec3(0,0,0), vec3(165,330,165), white));
    box1->rotate(15, transform_o::ROTATE_Y);
    box1->translate(vec3(265, 0, 295));

    auto box2 = make_shared<transform_o>(box(vec3(0,0,0), vec3(165,165,165), white));
    box2->rotate(-18, transform_o::ROTATE_Y);
    box2->translate(vec3(130, 0, 65));

    world.add(make_shared<constant_medium>(box1, 0.01, vec3(0,0,0)));
    world.add(make_shared<constant_medium>(box2, 0.01, vec3(1,1,1)));

    cf.aspect_ratio      = 1.0;
    cf.image_width       = 600;
    cf.aa_samples        = 200;
    cf.max_depth         = 50;

    cf.vfov     = 40;
    cf.pos      = vec3(278, 278, -800);
    cf.target   = vec3(278, 278, 0);
    cf.vup      = vec3(0,1,0);

    cf.defocus_angle = 0;

    return world;
}

hittable_list test(config& cf) {
    hittable_list world;

    auto perlin_texture = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_texture)));
    world.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    auto light = make_shared<emissive>(vec3(4));
    auto trans = make_shared<dielectric>(vec3(0.2, 0.6, 0.3), 1.5);
    world.add(make_shared<sphere>(vec3(0, 7, 0), 2, light));
    world.add(make_shared<sphere>(vec3(0, 7, 0), 2.3, trans));

    cf.image_width = 1024;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(26, 3, 6);
    cf.target = vec3(0, 2, 0);

    cf.defocus_angle = 0;

    return world;
}

hittable_list final_scene(config& cf) {
    hittable_list world;

    hittable_list boxes1;
    auto ground = make_shared<lambertian>(vec3(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_float(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground));
        }
    }

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<emissive>(vec3(7));
    world.add(make_shared<quad>(vec3(123,554,147), vec3(300,0,0), vec3(0,0,265), light));

    auto center1 = vec3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(vec3(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(vec3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        vec3(0, 150, 145), 50, make_shared<fuzzy>(vec3(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(vec3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, vec3(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(vec3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, vec3(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("images/earth.jpg"));
    world.add(make_shared<sphere>(vec3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(vec3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(vec3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(vec3::random(0,165), 10, white));
    }

    world.add(make_shared<transform_o>(
        make_shared<bvh_node>(boxes2))
            ->rotate(15, transform_o::ROTATE_Y)
            ->translate(vec3(-100,270,395))
    );

    cf.aspect_ratio      = 1.0;
    cf.image_width       = 400;
    cf.aa_samples        = 250;
    cf.max_depth         = 4;

    cf.vfov     = 40;
    cf.pos      = vec3(478, 278, -600);
    cf.target   = vec3(278, 278, 0);
    cf.vup      = vec3(0,1,0);

    cf.defocus_angle = 0;

    return world;
}

int main(int argc, char** argv) {
    InputParser input(argc, argv);
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help") || !input.valid()) {
        InputParser::helpMessage();
        return -1;
    }

    string output_file = input.getCmdOption("--out");
    bool save = !output_file.empty();

    bool window_display = input.cmdOptionExists("--display");

    bool tree = input.cmdOptionExists("--bvh");

    int scene = 0;
    string scene_str = input.getCmdOption("--scene");
    if (!scene_str.empty()) scene = stoi(scene_str);

    config cf;

    hittable_list world;
    switch (scene) {
        default:
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
            world = simple_light(cf);
            break;
        case 7:
            world = cornell_box(cf);
            break;
        case 8:
            world = cornell_smoke(cf);
            break;
        case 9:
            world = final_scene(cf);
            break;
    }

    configure(input, cf);

    camera cam(cf);
    if (tree) world = hittable_list(make_shared<bvh_node>(world));

    vector<uint8_t> pixels(cam.width() * cam.height() * 4);

    if (window_display) {
        thread render(&camera::render, &cam, std::ref(world), std::ref(pixels));

        sf::Image image(display(pixels, { (unsigned int)cam.width(), (unsigned int)cam.height() }));

        render.join();

        if (save) {
            bool success = image.saveToFile(output_file);
            if (success) cout << "Successfully created " << output_file << '\n';
            else cout << "Failed to write image\n";
        }
    } else {
        cam.render(world, pixels);

        if (save) {
            sf::Image image({ (unsigned int)cam.width(), (unsigned int)cam.height() }, pixels.data());
            bool success = image.saveToFile(output_file);
            if (success) cout << "Successfully created " << output_file << '\n';
            else cout << "Failed to write image\n";
        }
    }
}