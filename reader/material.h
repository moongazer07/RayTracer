#pragma once

#include "../raytracer/common_libs.h"
#include "../geometry/vector.h"

struct Material {
    Material() : specular_exponent(0)
                , refraction_index(0)
                , albedo{1,0,0} {}

    std::string name;
    Vector ambient_color;
    Vector diffuse_color;
    Vector specular_color;
    Vector intensity;
    double specular_exponent;
    double refraction_index;
    std::array<double, 3> albedo;
};
