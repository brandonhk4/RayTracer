#include <SFML/Graphics.hpp>
// Replace this with imGUI one day

#include <thread>

#include "scenes.h"

#include "utility/bvh.h"
#include "utility/InputParser.h"

#include "raytracer.h"
#include "camera.h"

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
            out = earth(cf);
            world = out.first;
            lights = out.second;
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
            out = cornell_smoke(cf);
            world = out.first;
            lights = out.second;
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
        case 11:
            out = scene_mirror(cf);
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