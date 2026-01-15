#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.141592653589793285;

// Utitlity Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    static std::mt19937 gen;
    return dist(gen);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// Common Headers

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "interval.h"

#endif