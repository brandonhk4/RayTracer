#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

inline float linear_to_gamma(float linear_component) {
    if (linear_component > 0) return std::sqrt(linear_component);
    return 0;
}

void write_color(std::ostream& out, const vec3& pixel_color) {
    float r = pixel_color.x;
    float g = pixel_color.y;
    float b = pixel_color.z;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.0, 0.999);
    unsigned char rbyte = (unsigned char)int(256 * intensity.clamp(r));
    unsigned char gbyte = (unsigned char)int(256 * intensity.clamp(g));
    unsigned char bbyte = (unsigned char)int(256 * intensity.clamp(b));

    out << rbyte << gbyte << bbyte;
}

void write_color(std::vector<std::uint8_t>::iterator& pixels, const vec3& pixel_color) {
    float r = pixel_color.x;
    float g = pixel_color.y;
    float b = pixel_color.z;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.0, 0.999);
    *pixels = static_cast<std::uint8_t>(std::floor(256 * intensity.clamp(r))); ++pixels;
    *pixels = static_cast<std::uint8_t>(std::floor(256 * intensity.clamp(g))); ++pixels;
    *pixels = static_cast<std::uint8_t>(std::floor(256 * intensity.clamp(b))); ++pixels;
    *pixels = static_cast<std::uint8_t>(255); ++pixels;
}

#endif