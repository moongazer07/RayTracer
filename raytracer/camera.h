#pragma once

#include "raytracerlib.h"
#include <math.h>

    // 1. Check wheathe ray intersects any object
    // 2. Depending on the mode, compute the color in a point
    //      I.  kNormal Mode
    //              The value of the ray is the value of normal in the point.
    //              Normal must be in the opposite direction of the ray
    //              All the values will be in the range from [-1, 1], in
    //              order to transform them -- use (0, 0, 0) in the points where ray 
    //              does not intersect scene
    //              and for vector v use 1/2 * v + 1/2
    //      II. kDepth Mode
    //              Result is the distance to the closes object to the ray (d, d, d)
    //              To normalize the values take the biggect value D, and all the other
    //              results transfer to d/D.
    //      III. kFull Mode
    //              I_p = I_base_p + I_comp_p
    //              I_base_p = K_a + K_e + al_0 * (Sum_{m \in lights} K_d * I_d(p, m) +
    //              K_s * I_s (p, m)) , I_d and I_s are diffuse and specular parts
    //              from Phong reflection model
    //              
    //              I_comp = al_1 I_reflect(p) + al_2 I_refract(p)
    //              Note: there is no need to compute I_reflect, if we trace the
    //              ray inside the object
    //             TO DO: 1) How to compute I_d? 
    //                    2) How to compute I_s?
    //                    3) How to compute I_reflect?
    //                    4) How to compude I_s?
    //                    5) Do we have to compute the shadows separetely form diffuse comp?
    //                    6) Make the diffuse randomized 
    //                    7) Add Antializing component
    //                    8) Add Tone mapping and Gamma correction
    //                    9) Implement Bounded Volume Hierarchies

double DegreesToRadians(double degrees);

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
         
        Camera(int screen_width, int screen_height, double fov, Vector look_from, Vector look_to) 
        : screen_width_(screen_width)
        , screen_height_(screen_height)
        , fov_(fov) 
        , look_from_(look_from)
        , look_to_(look_to) {
            double focal_length = Length(look_to_ - look_from_);
            double theta = DegreesToRadians(fov_);
            view_port_height_ = std::tan(theta/2) * focal_length * 2;
            view_port_width_ = view_port_height_ * static_cast<double>(screen_width_)/screen_height_;
            vup_ = Vector(0, 1, 0);
            Vector w{UnitVector(look_from_ - look_to_)};
            if (Length(CrossProduct(vup_, w)) < std::numeric_limits<float>::epsilon()) {
                vup_ = Vector(1, 0, 0);
            } 
            Vector u{UnitVector(CrossProduct(w, vup_))};
            Vector v{CrossProduct(u, w)};
            Vector view_port_horizontal{view_port_width_ * u};
            Vector view_port_vertical{view_port_height_ * -v};
            view_port_horizontal_del_ = view_port_horizontal / screen_width_;
            view_port_vertical_del_ = view_port_vertical / screen_height_;
            Vector view_port_upperleft{look_from_ - (focal_length * w) - (view_port_horizontal + view_port_vertical) / 2.0};
            view_port_00_pixel_ = view_port_upperleft + 0.5 * (view_port_vertical_del_ + view_port_horizontal_del_);
        }
};

double DegreesToRadians(double degrees) {
            return M_PI/180 * degrees;
        }

void write_color(const Vector& pixel_color) {
    int r = pixel_color[0] * 255.999;
    int g = pixel_color[1] * 255.999;
    int b = pixel_color[2] * 255.999;
    r = g - b;
    r = +r;
    // std::cout << r << ' ' << g << ' ' << b << '\n';
}

std::optional<Intersection> CheckIntersection(const Ray& ray, const Scene& scene) {
    double dist = std::numeric_limits<double>::max();
    std::optional<Intersection> intersection = std::nullopt;
    for (auto sphere : scene.GetSphereObjects()) {
        std::optional<Intersection> int_with_sphere = GetIntersection(ray, sphere.sphere);
        if (int_with_sphere != std::nullopt && int_with_sphere->GetDistance() < dist) {
            dist = int_with_sphere->GetDistance();
            intersection = int_with_sphere;
        }
    }
    for (auto poly : scene.GetObjects()) {
        std::optional<Intersection> int_with_poly = GetIntersection(ray, poly.polygon);
        if (int_with_poly != std::nullopt) {
            // std::cout << "Intersected the material:" << poly.material->name << "\n";
            std::cout << "Point of intersection: ";
            PrintVec(int_with_poly->GetPosition());
            std::cout << "With distance: " << int_with_poly->GetDistance() << "\n";
            std::cout << "Closest distance: " << intersection->GetDistance() << "\n";
        }
        if (int_with_poly != std::nullopt && int_with_poly->GetDistance() < dist) {
            dist = int_with_poly->GetDistance(); 
            intersection = int_with_poly;
            // std::cout << "Current material: " << poly.material->name << "\n";
        }
    }
    return intersection;
}

Vector ComputeColorNormal(const Ray& ray, const Scene& scene) {
    std::optional<Intersection> intersection = CheckIntersection(ray, scene);
    if (intersection == std::nullopt) {
        return {0, 0, 0};
    }
    Vector normal = intersection->GetNormal();
    normal = normal * 0.5 + 0.5;

    return normal;
}

void Render(const Camera& camera, const Scene& scene, const RenderOptions& render_options) {

    for (int y = 0; y < camera.screen_height_; ++y) {
        for (int x = 0; x < camera.screen_width_; ++x) {
            auto pixel_center = camera.view_port_00_pixel_ + (camera.view_port_horizontal_del_ * x) + (camera.view_port_vertical_del_ * y);
            Ray ray(camera.look_from_, pixel_center - camera.look_from_);
            Vector pixel_color;
            switch (render_options.mode)
            {
            // case RenderMode::kDepth :
            //     pixel_values = ComputeColorDepth(ray, scene, &max_dist);
            //     break;
            case RenderMode::kNormal :
                pixel_color = ComputeColorNormal(ray, scene);
            // case RenderMode::kFull :
            //     pixel_color = ComputeColorFull(ray, scene, render_options.depth);
            default:
                break;
            }
            write_color(pixel_color);
        }

    }
}


// Vector ComputeColorDepth(const Ray& ray, const Scene& scene, double* max_dist) {
//     std::optional<Intersection> intersection = CheckIntersection(ray, scene);
//     if (intersection = std::nullopt) {
//         return {1, 1, 1};
//     }

//     double dist = intersection->GetDistance();
//     if (dist > *max_dist) {
//         *max_dist = intersection->GetDistance();
//     }
//     return {dist, dist, dist};
// }

// RGB ComputeColorFull(const Ray& ray, const Scene& scene, int depth) {

// }