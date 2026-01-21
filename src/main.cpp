#include <SFML/Graphics.hpp>

#include "OpenCLHelper.h"

#include "raytracer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "bvh.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"

using namespace std;

class InputParser{
    private:
        vector <string> tokens;

        static vector<string> options;

    public:
        int argc;

        InputParser (int &argc, char **argv) : argc(argc) {
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(string(argv[i]));
        }

        const string& getCmdOption(const string &option) const{
            vector<string>::const_iterator itr;
            itr =  find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const string empty_string("");
            return empty_string;
        }

        bool cmdOptionExists(const string &option) const{
            return find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }

        static void helpMessage() {
            cout << "Configurations Options:\n";
            cout << "\t\t--f (output file)\n";
            for (auto it = options.begin(); it != options.end(); ++it) {
                cout << "\t\t" << *it << '\n';
            }
        }
};

vector<string> InputParser::options = {
            "--aspect_ratio",
            "--image_width",
            "--aa_samples",
            "--max_depth",
            "--field_of_view",
            "--position",
            "--target",
            "--vertical_up",
            "--defocus_angle",
            "--focus_distance"
        };

config configure(const InputParser& input) {
    config cf;
    if (input.argc <= 1) return cf;

    const string aspect_ratio_str = input.getCmdOption("--aspect_ratio");
    if (!aspect_ratio_str.empty()) cf.aspect_ratio = stod(aspect_ratio_str);
    
    const string image_width_str = input.getCmdOption("--width");
    if (!image_width_str.empty()) cf.image_width = stoi(image_width_str);

    const string aa_samples_str = input.getCmdOption("--aa_samples");
    if (!aa_samples_str.empty()) cf.aa_samples = stoi(aa_samples_str);

    const string max_depth_str = input.getCmdOption("--max_depth");
    if (!max_depth_str.empty()) cf.max_depth = stoi(max_depth_str);

    const string vfov_str = input.getCmdOption("--field_of_view");
    if (!vfov_str.empty()) cf.vfov = stod(vfov_str);

    const string pos_str = input.getCmdOption("--position");
    if (!pos_str.empty()) cf.pos = vec3::stov(pos_str);

    const string target_str = input.getCmdOption("--target");
    if (!target_str.empty()) cf.target = vec3::stov(target_str);

    const string vup_str = input.getCmdOption("--vertical_up");
    if (!vup_str.empty()) cf.vup = vec3::stov(vup_str);

    const string defocus_angle_str = input.getCmdOption("--defocus_angle");
    if (!defocus_angle_str.empty()) cf.defocus_angle = stod(defocus_angle_str);

    const string focus_dist_str = input.getCmdOption("--focus_distance");
    if (!focus_dist_str.empty()) cf.focus_dist = stod(focus_dist_str);

    return cf;
}

int main(int argc, char** argv) {
    InputParser input(argc, argv);
    if (input.cmdOptionExists("-h")) {
        InputParser::helpMessage();
        return -1;
    }

    config cf = configure(input);

    cf.image_width = 600;
    cf.aa_samples = 50;
    cf.max_depth = 16;

    cf.vfov = 20;
    cf.pos = vec3(13, 2, 3);
    cf.target = vec3();
    
    cf.defocus_angle = 0.6;
    cf.focus_dist = 10.0;

    camera cam(cf);
    
    // // OpenCLHelper gpu;

    // // float *ray_pts_h = new float[cam.width() * cam.height() * cam.aa_samples * 4];
    // // float *ray_dirs_h = new float[cam.width() * cam.height() * cam.aa_samples * 4];
    // // float *out_h = new float[cam.width() * cam.height() * cam.aa_samples * 4];

    // // cam.generate_rays(ray_pts_h, ray_dirs_h);
    
    // // auto ray_pts_d = gpu.make_image(ray_pts_h, cam.width(), cam.height(), cam.aa_samples, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    // // auto ray_dirs_d = gpu.make_image(ray_dirs_h, cam.width(), cam.height(), cam.aa_samples, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    // // auto out_d = gpu.make_image(out_h, cam.width(), cam.height(), cam.aa_samples, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR);
    
    // // gpu.run("ray_color", cam.width(), cam.height(), cam.aa_samples, ray_pts_d, ray_dirs_d, out_d);
    
    // // gpu.read_image(out_d, out_h, cam.width(), cam.height(), cam.aa_samples);

    string output_file = input.getCmdOption("--out");
    bool save = !output_file.empty();

    bool tree = input.cmdOptionExists("--bvh");

    // World
    hittable_list world;

    shared_ptr<lambertian> ground_mat = make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
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

    if (tree) world = hittable_list(make_shared<bvh_node>(world));
    vector<uint8_t> pixels;
    pixels.reserve(cam.width() * cam.height() * 4);
    // // for (int i = 0; i < cam.width() * cam.height() * 4; ++i) {
    // //     pixels.push_back(uint8_t(out_h[i] * 255));
    // // }

    // // delete[] ray_pts_h;
    // // delete[] ray_dirs_h;
    // // delete[] out_h;
    cam.render(world, pixels);

    sf::RenderWindow window(sf::VideoMode({ (unsigned int)cam.width(), (unsigned int)cam.height() }), "RayTracer", sf::Style::Default, sf::State::Windowed);

    sf::Image image({(unsigned int)cam.width(), (unsigned int)cam.height()}, pixels.data());
    if (save) {
        bool success =image.saveToFile(output_file);
        if (success) cout << "Successfully created image.png\n";
        else cout << "Failed to write image\n";
    }

    sf::Texture texture(image);

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
    }
}