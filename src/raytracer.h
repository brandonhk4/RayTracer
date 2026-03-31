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
const float pi = 3.141592653589793285f;

// Utitlity Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0f;
}

inline float random_float() {
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
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

#include "math/vec3.h"
#include "math/ray.h"
#include "utility/color.h"
#include "utility/interval.h"

#endif