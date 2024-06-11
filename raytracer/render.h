#pragma once

#include "raytracerlib.h"
#include "camera.h"

std::pair<std::optional<Intersection>, std::shared_ptr<Material>> CheckIntersection(const Ray& ray, const Scene& scene) {
    double dist = std::numeric_limits<double>::max();
    std::shared_ptr<Material> material = nullptr;
    std::optional<Intersection> result_intersection = std::nullopt;
    for (const auto& object : scene.GetObjects()) {
        auto intersection = object->GetIntersection(ray);
        if (intersection != std::nullopt && intersection->GetDistance() < dist) {
            dist = intersection->GetDistance();
            result_intersection = intersection;
            material = object->GetMaterial();
        }
    }
    return {result_intersection, material};
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
            color *= camera.sample_per_pixel_scale_;
            image_buffer[j].push_back(color);
        }
    }
    double max = FindMax(image_buffer, camera);
    DivideTemplate(&image_buffer, camera, max);
}