#pragma once

#include "../raytracerlib.h"

struct Box { 

    public:

    Box() : bottom_(Vector(-std::numeric_limits<double>::infinity()))
          , top_(Vector(std::numeric_limits<double>::infinity()))
          {}

    Box(const Vector& bottom, const Vector& top) {
        bottom_ = bottom;
        top_ = top;
        ComputeCentroid();
    }

    Box(const Box& other) {
        bottom_ = other.bottom_;
        top_ = other.top_;
        centroid_ = other.centroid_;
    }

    Box(const Triangle& triangle) {
        // compute the box for triangle :
        // for every point
        // project x on normal(1,0,0)
        // project y on normal(0,1,0)
        // project z on normal(0,0,1)
        // take maximal x,y,z
        // take minimal x,y,z
        // max x, y, z will be top
        // min x, y, z will be bottom
        double x_min = triangle.GetVertex(0)[0];
        double x_max = triangle.GetVertex(0)[0];
        double y_min = triangle.GetVertex(0)[1];
        double y_max = triangle.GetVertex(0)[1];
        double z_min = triangle.GetVertex(0)[2];
        double z_max = triangle.GetVertex(0)[2];
        for (size_t vertex = 1; vertex < 3; ++vertex) {
            x_min = std::min(x_min, triangle.GetVertex(vertex)[0UL]);
            x_max = std::max(x_max, triangle.GetVertex(vertex)[0UL]);
            y_min = std::min(y_min, triangle.GetVertex(vertex)[1UL]);
            y_max = std::max(y_max, triangle.GetVertex(vertex)[1UL]);
            z_min = std::min(z_min, triangle.GetVertex(vertex)[2UL]);
            z_max = std::max(z_max, triangle.GetVertex(vertex)[2UL]);
        }
        bottom_ = {x_min, y_min, z_min};
        top_ = {x_max, y_max, z_max};
        ComputeCentroid();
    }

    Box(const Sphere& sphere) {
        // compute the box for sphere
        // min = center - radius
        // max = center + raidus
        bottom_ = sphere.GetCenter() - sphere.GetRadius();
        top_ = sphere.GetCenter() + sphere.GetRadius();
        ComputeCentroid();
    }

    Box& operator = (const Box& other) {
        bottom_ = other.bottom_;
        top_ = other.top_;
        centroid_ = other.centroid_;
        return *this;
    }

    const Vector& GetTop() const {
        return top_;
    }

    const Vector& GetBot() const {
        return bottom_;
    }

    const Vector& GetCentroid() const {
        return centroid_;
    }

    void ExtendBox(const Box& other) {
        // Vector& new_bot = new_box.bot;
        // Vector& new_top = new_box.top;
        // for every value i in bot_, top_
        // compare bot[i] with bigger_bot[i]
        // compare top[i] with bigger_top[i]
        // update if bigger_bot[i] is smaller
        // update if bigger_top[i] is bigger
        for (size_t i = 0; i < 3; ++i) {
            bottom_[i] = std::min(bottom_[i], other.GetBot()[i]);
            top_[i] = std::max(top_[i], other.GetTop()[i]);
        }
        ComputeCentroid();
    }

    private:

    void ComputeCentroid() {
       centroid_ = (bottom_ + top_) * 0.5;
    }

    Vector bottom_;
    Vector top_;
    Vector centroid_;

}; 

std::optional<std::pair<double, double>> IntersectBox(const Ray& ray, const Box& box) {
    Vector ray_direction = ray.GetDirection();
    Vector ray_origin = ray.GetOrigin();
    Vector bottom = box.GetBot();
    Vector top = box.GetTop();
    Vector denominator{1/ray_direction[0], 1/ray_direction[1], 1/ray_direction[2]};
    double tNear = -std::numeric_limits<double>::infinity();
    double tFurther = std::numeric_limits<double>::infinity();
    double t0 = 0;
    double t1 = 0;
    for (size_t ind = 0; ind < 3; ++ind) {
        if (ray_direction[ind] == 0 && ray_origin[ind] < box.GetBot()[ind] || box.GetTop()[ind] < ray_origin[ind]){
            return std::nullopt;
        }
        t0 = (bottom[ind] - ray_origin[ind]) * denominator[ind];
        t1 = (top[ind] - ray_origin[ind]) * denominator[ind];
        if (t1 < t0) {
            std::swap(t0, t1);  //in case ray coord was negative
        }
        if (tNear > t1 || tFurther < t0) {
            return std::nullopt;
        }
        if (tNear < t0) {
            tNear = t0;
        }
        if (t1 < tFurther) {
             tFar = t1;
        }
        if (tNear > tFurther) {
            return std::nullopt;
        }
    }
    return std::make_optional(std::make_pair(tNear, tFurther));
}