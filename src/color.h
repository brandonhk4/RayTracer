#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) return std::sqrt(linear_component);
    return 0;
}

void write_color(std::ostream& out, const vec3& pixel_color) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.0, 0.999);
    unsigned char rbyte = (unsigned char)int(256 * intensity.clamp(r));
    unsigned char gbyte = (unsigned char)int(256 * intensity.clamp(g));
    unsigned char bbyte = (unsigned char)int(256 * intensity.clamp(b));

    out << rbyte << gbyte << bbyte;
}

#endif