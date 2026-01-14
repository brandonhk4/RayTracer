#ifndef COLOR_H
#define COLOR_H

#include "raytracer.h"

void write_color(std::ostream& out, const vec3& pixel_color) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    unsigned char rbyte = (unsigned char)int(255.999 * r);
    unsigned char gbyte = (unsigned char)int(255.999 * g);
    unsigned char bbyte = (unsigned char)int(255.999 * b);

    out << rbyte << gbyte << bbyte;
}

#endif