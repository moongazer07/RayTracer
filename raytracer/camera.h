#pragma once

#include "raytracerlib.h"

struct Camera {

        int screen_width_;
        int screen_height_;
        double fov_;
        Vector look_from_;
        Vector look_to_;
        Vector vup_;
        double view_port_height_;
        double view_port_width_;
        Vector view_port_horizontal_del_;
        Vector view_port_vertical_del_;
        Vector view_port_00_pixel_;
         
        Camera(int screen_width, int screen_height, Vector look_from, Vector look_to, double fov) 
        : screen_width_(screen_width)
        , screen_height_(screen_height) 
        , look_from_(look_from)
        , look_to_(look_to)
        , fov_(fov) {
            double focal_length = Length(look_to_ - look_from_);
            double theta = DegreesToRadians(fov_);
            view_port_height_ = std::tan(theta / 2) * focal_length * 2;
            view_port_width_ = view_port_height_ * (static_cast<double>(screen_width_/screen_height_));
            vup_ = Vector(0, 1, 0);
            Vector w{UnitVector(look_from_ - look_to_)};
            Vector u{UnitVector(CrossProduct(vup_, w))};
            Vector v{CrossProduct(w, u)};
            Vector view_port_horizontal{view_port_width_ * u};
            Vector view_port_vertical{view_port_height_ * -v};
            view_port_horizontal_del_ = view_port_horizontal / screen_width_;
            view_port_vertical_del_ = view_port_vertical / screen_height_;
            Vector view_port_upperleft{look_from - (focal_length * w) - (view_port_horizontal + view_port_vertical) / 2.0};
            view_port_00_pixel_ = view_port_upperleft + 0.5 * (view_port_vertical_del_ + view_port_horizontal_del_);
        }
};

double DegreesToRadians(double degrees) {
            return M_PI/180 * degrees;
        }

RGB ComputeColor(const Ray& ray, const Scene& scene, int* depth, const RenderMode& mode) {
    
}

Image Render(const Camera& camera, const Scene& scene, const RenderOptions& render_options) {
    Image image{camera.screen_width_, camera.screen_height_};
    
    for (int y = 0; y < camera.screen_height_; ++y) {
        for (int x = 0; x < camera.screen_width_; ++x) {
            auto pixel_center = camera.view_port_00_pixel_ + (camera.view_port_horizontal_del_ * x) + (camera.view_port_vertical_del_ * y);
            Ray ray(camera.look_from_, pixel_center - camera.look_from_);
            RGB pixel_color = ComputeColor(ray, scene, &render_options.depth, render_options.mode);
            image.SetPixel(pixel_color, y, x);
        }
    }
    return image;
}