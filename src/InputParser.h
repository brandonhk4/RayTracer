#ifndef PARSER_H
#define PARSER_H

#include "camera.h"

using namespace std;

class InputParser{
    private:
        vector<string> tokens;

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
            cout << "\t\t--out (output file)\n";
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
            "--focus_distance",
            "--background"
        };

void configure(const InputParser& input, config& cf) {
    if (input.argc <= 1) return;

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

    const string background_str = input.getCmdOption("--background");
    if (!background_str.empty()) cf.background = vec3::stov(background_str);
}

#endif