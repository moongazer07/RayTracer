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
Vector ComputeColorFull(const Ray& ray, const Scene& scene, int depth, double refraction_index);

struct CameraToWorld {
    Vector right_;
    Vector forward_;
    Vector true_up_;
    Vector look_from_;

    CameraToWorld(const Vector& look_from, const Vector& look_to) 
    : forward_(UnitVector(look_from - look_to))
    , look_from_(look_from) {
        auto epsilon = std::numeric_limits<float>::epsilon();
        Vector up{0, 1, 0};

        if (Length(CrossProduct(forward_, up)) < epsilon) {
            up = {1, 0, 0};
        }
        right_ = UnitVector(CrossProduct(up, forward_));
        true_up_ = UnitVector(CrossProduct(forward_, right_));
    }
};


struct Camera {
        size_t screen_width_;
        size_t screen_height_;
        double aspect_ratio_;
        double scale_;
        CameraToWorld camera_to_world_;
        Vector new_look_from_;
        int samples_per_pixel_{10};
        double sample_per_pixel_scale_;

        Camera (size_t screen_width, size_t screen_height, double fov, const Vector& look_from, const Vector& look_to) 
        : screen_width_(screen_width)
        , screen_height_(screen_height)
        , aspect_ratio_(static_cast<double>(screen_width_) / screen_height_)
        , scale_(std::tan(DegreesToRadians(fov / 2))) 
        , camera_to_world_(look_from, look_to)
        , new_look_from_(look_from)
        {};

        Vector ApplyMatrix(const Vector& standart) const {
            double x = camera_to_world_.right_[0] * standart[0] +
                       camera_to_world_.true_up_[0] * standart[1] +
                       camera_to_world_.forward_[0] * standart[2];

            double y = camera_to_world_.right_[1] * standart[0] +
                       camera_to_world_.true_up_[1] * standart[1] +
                       camera_to_world_.forward_[1] * standart[2];

            double z = camera_to_world_.right_[2] * standart[0] +
                       camera_to_world_.true_up_[2] * standart[1] +
                       camera_to_world_.forward_[2] * standart[2];

            return {x, y, z}; 
        }
};

double DegreesToRadians(double degrees) {
    return M_PI/180 * degrees;
}

void write_color(const Vector& pixel_color) {
    int r = pixel_color[0] * 255.999;
    int g = pixel_color[1] * 255.999;
    int b = pixel_color[2] * 255.999;
    std::cout << r << ' ' << g << ' ' << b << '\n';
}

std::pair<std::optional<Intersection>, const Material*> CheckIntersection(const Ray& ray, const Scene& scene) {
    double dist = std::numeric_limits<double>::max();
    const Material* material = nullptr;
    std::optional<Intersection> intersection = std::nullopt;
    for (auto sphere : scene.GetSphereObjects()) {
        auto int_with_sphere = GetIntersection(ray, sphere.sphere);
        if (int_with_sphere != std::nullopt && int_with_sphere->GetDistance() < dist) {
            dist = int_with_sphere->GetDistance();
            intersection = int_with_sphere;
            material = sphere.material;
        }
    }
    for (auto poly : scene.GetObjects()) {
        auto int_with_poly = GetIntersection(ray, poly.polygon);
        if (int_with_poly != std::nullopt && int_with_poly->GetDistance() < dist) {
            dist = int_with_poly->GetDistance(); 
            intersection = int_with_poly;
            material = poly.material;
        }
    }
    return {intersection, material};
}

double ComputeDiffuse(const Vector& point_to_light, const Vector& normal) {
    return DotProduct(point_to_light, normal) < 0 ? 0 : DotProduct(point_to_light, normal);
}

double ComputeSpecular(const Vector& point_to_light, const Vector& normal, const Vector& viewer_direction, int exponent) {
    Vector reflection = UnitVector(Reflect(-point_to_light, normal));
    double cos = DotProduct(reflection, -viewer_direction);
    if (cos <= 0) {
        return 0;
    }
    return std::pow(cos, exponent);
}

Vector ComputeReflection(const Intersection& intersection, const Ray& prev_ray, const Scene& scene, int depth) {
    Vector reflection_dir = Reflect(prev_ray.GetDirection(), intersection.GetNormal());
    Ray reflected_ray{intersection.GetPosition(), reflection_dir};
    return ComputeColorFull(reflected_ray, scene, depth, 1);
}

Vector ComputeRefraction(const Intersection& intersection, const Ray& prev_ray, const Scene& scene,  
                                                                                double env_refraction_indx,
                                                                                double mat_refraction_indx, 
                                                                                                int depth) {
    std::optional<Vector> refraction_dir = Refract(prev_ray.GetDirection(), intersection.GetNormal(), env_refraction_indx / mat_refraction_indx);
    if (!refraction_dir.has_value()) {
        return {0, 0, 0};
    }
    Ray refracted_ray{intersection.GetPosition(), refraction_dir.value()};
    return ComputeColorFull(refracted_ray, scene, depth, mat_refraction_indx);
}

std::pair<Vector, Vector> ComputeDiffusiveSpecular (const Scene& scene, const Ray& prev_ray, 
                                                    const Intersection& intersection, double specular_exp) {
    Vector diffuse_component, specular_component, point_to_light;
    Vector point_intersection = intersection.GetPosition();
    Vector normal = intersection.GetNormal();
    for (auto light : scene.GetLights()) {
        point_to_light = light.position - point_intersection;
        auto point_to_light_len = Length(point_to_light);
        auto point_to_light_dir = UnitVector(point_to_light);
        auto light_check = CheckIntersection({point_intersection, point_to_light_dir}, scene).first;
        if (!light_check.has_value() || light_check->GetDistance() > point_to_light_len) {
            diffuse_component += light.intensity * ComputeDiffuse(point_to_light, normal); 
            specular_component += light.intensity * ComputeSpecular(point_to_light, normal, 
                                                                prev_ray.GetDirection(), specular_exp);
        }
    }
    return {diffuse_component, specular_component};
}

Vector ComputeColorFull(const Ray& ray, const Scene& scene, int depth, double refraction_index) {
    if (depth < 1) {
        return {0, 0, 0};
    }
    auto [intersection, material] = CheckIntersection(ray, scene);
    if (!intersection.has_value() || material == nullptr) {
        return {0, 0, 0};
    }
    Vector albedo = material->albedo;
    Vector diffuse_component, specular_component;
    if (albedo[0]) {
        std::tie(diffuse_component, specular_component)  = ComputeDiffusiveSpecular(scene, ray, intersection.value(), material->specular_exponent);
    }
    Vector phong_diff = material->diffuse_color * diffuse_component;
    Vector phong_spec = material->specular_color * specular_component;
    Vector reflection, refraction;
    if (albedo[1] && refraction_index == 1) {
        reflection = ComputeReflection(intersection.value(), ray, scene, depth - 1);
    }
    if (albedo[2]) {
        refraction = ComputeRefraction(intersection.value(), ray, scene, refraction_index, material->refraction_index, depth - 1);
    }
    return material->ambient_color + material->intensity + albedo[0] * (phong_diff + phong_spec) + albedo[1] * reflection + albedo[2] * refraction;
}

Vector ComputeColorNormal(const Ray& ray, const Scene& scene) {
    auto [intersection, material] = CheckIntersection(ray, scene);
    if (intersection == std::nullopt) {
        return {0, 0, 0};
    }
    Vector normal = intersection->GetNormal();
    normal = normal * 0.5 + 0.5;
    return normal;
}

double FindMax(std::vector<std::vector<Vector>> templat, const Camera& camera) {
    double max = 0;
    
    for (size_t y = 0; y < camera.screen_height_; ++y) {
        for (size_t x = 0; x < camera.screen_width_; ++x) {
            for (int i = 0; i < 3; ++i) {
                if (max < templat[y][x][i]) {
                    max = templat[y][x][i];
                }
            }
        }
    }
    return max != 0 ? max : 1;
}

void DivideTemplat(std::vector<std::vector<Vector>>* templat, const Camera& camera, double max) {
    for (size_t y = 0; y < camera.screen_height_; ++y) {
        for (size_t x = 0; x < camera.screen_width_; ++x) {
            Vector frac_above = (*templat)[y][x] * (1 + ((*templat)[y][x] / max * max));
            Vector frac_below = (*templat)[y][x] + 1;
            Vector out = frac_above / frac_below;
            out[0] = std::pow(out[0], 1/2.2);
            out[1] = std::pow(out[1], 1/2.2);
            out[2] = std::pow(out[2], 1/2.2);
            write_color(out);
        }
    }
}

Vector ComputeDirection(size_t i, size_t j, const Camera& camera) {
    auto offset_x = RandomDouble() - 0.5;
    auto offset_y = RandomDouble() - 0.5;
    auto x = (2 * ((static_cast<double>(i) + 0.5 + offset_x) / static_cast<double>(camera.screen_width_)) - 1) * camera.aspect_ratio_ * camera.scale_;
    auto y = (1 - 2 * ((static_cast<double>(j) + 0.5 + offset_y) /static_cast<double>(camera.screen_height_))) * camera.scale_;
    return UnitVector(camera.ApplyMatrix({x, y, -1}));
}

void Render(const Camera& camera, const Scene& scene, const RenderOptions& render_options) {
    std::vector<std::vector<Vector>> image_buffer;
    image_buffer.resize(camera.screen_height_);
    for (size_t j = 0; j < camera.screen_height_; ++j) {
        for (size_t i = 0; i < camera.screen_width_; ++i) {
            Vector color;
            for (int sample = 0; sample < camera.samples_per_pixel_; ++sample) {
                auto direction = ComputeDirection(i, j, camera);
                Ray ray(camera.new_look_from_, direction);
                    switch (render_options.mode) {
                        case RenderMode::kNormal :
                            color += ComputeColorNormal(ray, scene);
                            break;
                        case RenderMode::kFull :
                            color += ComputeColorFull(ray, scene, render_options.depth, 1);
                            break;
                        default:
                            break;
                    }
            }
            color /= camera.samples_per_pixel_;
            image_buffer[j].push_back(color);
        }
    }
    double max = FindMax(image_buffer, camera);
    DivideTemplat(&image_buffer, camera, max);
}