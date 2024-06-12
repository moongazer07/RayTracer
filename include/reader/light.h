#pragma once

#include "../raytracer/common_libs.h"
#include "../geometry/vector.h"

struct Light {
    Vector position;
    Vector intensity;

    Light(Vector position, Vector intensity)
    : position(std::move(position))
    , intensity(std::move(intensity)) {}

};
