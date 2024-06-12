#pragma once

#include "../raytracer/common_libs.h"
#include "vector.h"

class Ray {
public:
    Ray(const Vector& origin, const Vector& direction) : origin_(origin)
                                         , direction_(direction) {
        direction_.Normalize();   
    }
                                         
    const Vector& GetOrigin() const {
        return origin_;
    }

    Vector GetOrigin() {
        return origin_;
    }

    const Vector& GetDirection() const {
        return direction_;
    }

    Vector GetDirection() {
        return direction_;
    }

    const Vector at (double r) const {
        return origin_ + (direction_ * r);
    }

    Vector operator() (double r) {
        return origin_ + (direction_ * r);
    } 

private:
    Vector origin_;
    Vector direction_;
};
