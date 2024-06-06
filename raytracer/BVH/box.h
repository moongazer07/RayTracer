
#include "../raytracerlib.h"

struct Box { 

    Box() : bottom_(Vector(std::numeric_limits<double>::max()))
          , top_(Vector(std::numeric_limits<double>::max()))
          , centroid_((bottom_ + top_) * 0.5)
          {}

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
    }

    Box(const Sphere& sphere) {
        // compute the box for sphere
        // min = center - radius
        // max = center + raidus
    }

    void ExtendTheBox(const Vector& bigger_bot, const Vector& bigger_top) {
        // for every value i in bot_, top_
        // compare bot[i] with bigger_bot[i]
        // compare top[i] with bigger_top[i]
        // update if bigger_bot[i] is smaller
        // update if bigger_top[i] is bigger
    }

    Vector bottom_;
    Vector top_;
    Vector centroid_;

}; 

std::optional<std::pair<double, double>> IntersectBox(const Ray& ray, const Box& box) {
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
}