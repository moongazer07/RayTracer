#pragma once

#include "../raytracerlib.h"

struct Box { 

    public:

    Box() : bottom_(Vector(std::numeric_limits<double>::min()))
          , top_(Vector(std::numeric_limits<double>::max()))
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

// std::optional<std::pair<double, double>> IntersectBox(const Ray& ray, const Box& box) {
    // Ray_coord = 1 / Ray_dir
    // tNear == max of mins
    // tFar == min of maxs
    // for every index 0 .. 2
    //  compute t0 and t1
    //  if (Ray_dir[i] == 0 && Orig[i] < bottom_[i] || top_[i] < Orig[0])
    //            no intersection
    //  t0 = (coordinate_min - Orig_coord) * Ray_coord
    //  t1 = (coordinate_max - Orig_coord) * Ray_coord
    //  if t1 < t0 :: swap(t0, t1) -- in case ray coord was negative
    //  if tNear > t1 || tFar < t0 : return std::null_opt
    //  if t0 > tNear : tNear = t0
    //  if t1 < tFar : tFar = t1
    // if t_end < t_start => no intersection
// }