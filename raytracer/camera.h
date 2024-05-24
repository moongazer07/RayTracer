#pragma once

#include "raytracerlib.h"

class Camera {
    public: 

        Camera(int screen_width, int screen_height, Vector look_from, Vector look_to, double fov) 
        : screen_width_(screen_width)
        , screen_height_(screen_height) 
        , look_from_(look_from)
        , look_to_(look_to)
        , fov_(fov) {
            auto focal_length = Length(look_to_ - look_from_);
            auto theta = DegreesToRadians(fov_);
            auto h = std::tan(theta / 2);
            auto viewport_height = 2 * h * focal_length;
            auto viewport_width = viewport_height * (static_cast<double>(screen_width_/screen_height_))
            // add viewport characteristics
            // add viewport private memeber
            
        }

    private:

        double DegreesToRadians(double degrees) const {
            return M_PI/18 * degrees;
        }

        int screen_width_;
        int screen_height_;
        double fov_;
        Vector look_from_;
        Vector look_to_;
        Vector pixel00_loc_;
        Vector pixel_delta_u_;
        Vector pixel_delta_v_;


};

Image Render(const Camera& camera, const Scene& scene, const RenderOptions& render_options) {

}