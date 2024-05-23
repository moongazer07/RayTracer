#pragma once

// #include "../raytracer-geom/geometry.h"
// #include "material.h"

#include <triangle.h>
#include <material.h>
#include <sphere.h>


#include "scene_utils.h"

struct Object {

    Object(const Material* mat ,std::array<Vector, 3> pt1, std::array<Vector, 3> pt2, std::array<Vector, 3> pt3) :
      material(mat)
    , polygon(pt1[0], pt2[0], pt3[0])
    , textures(pt1[1], pt2[1], pt3[1])
    , normals(pt1[2], pt2[2], pt3[2]) {}

    const Material *material;
    Triangle polygon;
    Triangle textures;
    Triangle normals;

    const Vector* GetNormal(size_t index) const {
        return &normals.GetVertex(index);
    }
};

struct SphereObject {
    const Material *material = nullptr;
    Sphere sphere;
};
