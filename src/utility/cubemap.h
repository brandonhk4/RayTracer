#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "rtw_stb_image.h"
#include "../math/ray.h"

#include <iostream>

class cubemap {
    private:
        rtw_image sides[6]; // in order of negx, posx, negy, posy, negz, posz;
        bool flag = false;

        vec3 getPixelColor(int i, int j, const rtw_image& side) const {
            if (i >= side.width()) i = side.width() - 1;
            if (j >= side.height()) j = side.height() - 1;
            const unsigned char* pixel = side.pixel_data(i, j);
            float color_scale = 1.0f / 255.0f;
            return vec3(pixel[0], pixel[1], pixel[2]) * color_scale;
        }

    public:
        cubemap (const char* image_filename) {
            std::string filename = std::string(image_filename);
            if (filename.empty()) return;
            std::cout << "Trying to load cubemaps\n";
            if (!sides[0].load(filename + "/negx.png")) return;
            if (!sides[1].load(filename + "/posx.png")) return;
            if (!sides[2].load(filename + "/negy.png")) return;
            if (!sides[3].load(filename + "/posy.png")) return;
            if (!sides[4].load(filename + "/negz.png")) return;
            if (!sides[5].load(filename + "/posz.png")) return;
            std::cout << "Cubemap loaded successfully\n";
            flag = true;
        }

        explicit operator bool() const { return flag; }

        vec3 value (const ray& r) const {
            int axis = 0;
            const vec3& dir = r.dir();
            float big = std::fabs(dir.x);
            for (int i = 1; i < 3; ++i) {
                if (std::fabs(dir[i]) > big){
                    axis = i;
                    big = std::fabs(dir[i]);
                }
            }

            float u, v;

            switch (axis) {
                case 0:
                    u = -dir.z / dir.x;
                    v = -dir.y / big;
                    break;
                case 1:
                    u = dir.x / big;
                    v = dir.z / dir.y;
                    break;
                case 2:
                    u = dir.x / dir.z;
                    v = -dir.y / big;
                    break;
            }

            int side_i = axis * 2 + (dir[axis] > 0);
            u = (u + 1.0f) * 0.5f;
            v = (v + 1.0f) * 0.5f;

            const rtw_image& side = sides[side_i];

            u *= side.width();
            v *= side.height();
            int i = int(u);
            int j = int(v);

            vec3 col0 = getPixelColor(    i, j,     side);
            vec3 col1 = getPixelColor(i + 1, j,     side);
            vec3 col2 = getPixelColor(    i, j + 1, side);
            vec3 col3 = getPixelColor(i + 1, j + 1, side);

            float wi = u - i;
            float wj = v - j;

            // return col0;
            return (wi * wj) * col0 + ((1 - wi) * wj) * col1 + (wi * (1 - wj)) * col2 + ((1 - wi) * (1 - wj)) * col3;
        }
};

#endif