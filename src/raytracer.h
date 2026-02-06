#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

using std::make_shared;
using std::shared_ptr;

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.141592653589793285;

// Utitlity Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

inline float random_float() {
    static std::uniform_real_distribution<float> dist(0.0, 1.0);
    static std::mt19937 gen;
    return dist(gen);
}

inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
    return int(random_float(min, max));
}

// Common Headers

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "interval.h"

#endif