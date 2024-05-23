#pragma once

// #include <vector.h>
// #include <sphere.h>
// #include <intersection.h>
// #include <triangle.h>
// #include <ray.h>
// #include <limits>

#include "vector.h"
#include "sphere.h"
#include "intersection.h"
#include "triangle.h"
#include "ray.h"

#include <utility>
#include <optional>

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    auto oc = sphere.GetCenter() - ray.GetOrigin();
    auto tc = DotProduct(oc, ray.GetDirection());
    if (tc < 0) {
        return std::nullopt;
    }

    auto d = Length(oc) * Length(oc) - tc * tc;
    auto rad_2 = sphere.GetRadius() * sphere.GetRadius();

    if (d > rad_2) {
        return std::nullopt;
    }

    auto t1c = std::sqrt(rad_2 - d);
    auto t1 = std::abs(tc - t1c);
    auto IntersectionPoint = ray.at(t1);
    auto normal = (IntersectionPoint - sphere.GetCenter());
    normal.Normalize();
    if (DotProduct(normal, ray.GetDirection()) > 0) {
        normal = -normal;
    }
    auto distance = Length(IntersectionPoint - ray.GetOrigin());
    return Intersection(IntersectionPoint, normal, distance);
}

static inline double det3x3(const Vector& vec0, const Vector& vec1, const Vector& vec2) {
    Vector row0{vec0[0], vec1[0], vec2[0]};
    Vector row1{vec0[1], vec1[1], vec2[1]};
    Vector row2{vec0[2], vec1[2], vec2[2]};
    return DotProduct(row0, CrossProduct(row1, row2));
}

// static inline double det2x2(const Vector& vec0, const Vector& vec1, const Vector& vec2) {
//     Vector row0{vec0[0], vec1[0], vec2[0]};
//     Vector row1{vec0[1], vec1[1], vec2[1]};
//     Vector row2{vec0[2], vec1[2], vec2[2]};
//     return DotProduct(row0, CrossProduct(row1, row2));
// }

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    Vector constant_terms = ray.GetOrigin() - triangle.GetVertex(0);
    Vector vec_u = triangle.GetVertex(1) - triangle.GetVertex(0);
    Vector vec_v = triangle.GetVertex(2) - triangle.GetVertex(0);
    Vector normal = CrossProduct(vec_v, vec_u);
    normal.Normalize();
    if (DotProduct(normal, ray.GetDirection()) == 0) {
        return std::nullopt;
    }
    if (DotProduct(normal, ray.GetDirection()) > 0) {
        normal = -normal;
    } 
    double denominator = det3x3(-ray.GetDirection(), vec_u, vec_v);
    double t = det3x3(constant_terms, vec_u, vec_v) / denominator;
    double beta = det3x3(-ray.GetDirection(), constant_terms, vec_v) / denominator;
    if (beta < 0 || 1 < beta) {
        return std::nullopt;
    }
    double gamma = det3x3(-ray.GetDirection(), vec_u, constant_terms) / denominator;
    if (gamma < 0 || 1 + epsilon < beta + gamma) {
        return std::nullopt;
    }
    Vector IntersectionPoint = ray.at(t);
    double distance = Length(IntersectionPoint - ray.GetOrigin());
    return std::make_optional(Intersection(IntersectionPoint, normal, distance));
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    double cos_theta_i = DotProduct(-ray, normal);
    double cos_theta_T_2 = 1 - eta * eta * (1 - cos_theta_i * cos_theta_i);
    
    if (cos_theta_T_2 < 0) {
        return std::nullopt;
    }
    return (cos_theta_i * eta - std::sqrt(cos_theta_T_2)) * normal + ray * eta;

}

Vector Reflect(const Vector& ray, const Vector& normal) {
    return ray - 2 * DotProduct(ray, normal) * normal;
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    double area = triangle.Area();
    double alpha = Triangle(point, triangle.GetVertex(1), triangle.GetVertex(2)).Area() / area;
    double beta = Triangle(point, triangle.GetVertex(2), triangle.GetVertex(0)).Area() / area;
    double gamma = 1.0 - alpha - beta;
    return {alpha, beta, gamma};

}

