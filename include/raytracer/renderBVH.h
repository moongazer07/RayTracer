#pragma once

#include "common_libs.h"
#include "logs.h"
#include "../geometry/geometry.h"
#include "../reader/scene.h"
#include "render_options.h"
#include "camera.h"
#include "BVH/tree.h"
#include "image.h"

Vector ComputeColorFull(const Ray& ray, const Scene& scene, const BVH& bvh, int depth, double refraction_index);

Vector ComputeReflection(const Intersection& intersection, const Ray& prev_ray, const Scene& scene, const BVH& bvh, int depth) {
    Vector reflection_dir = Reflect(prev_ray.GetDirection(), intersection.GetNormal());
    Ray reflected_ray{intersection.GetPosition(), reflection_dir};
    return ComputeColorFull(reflected_ray, scene, bvh, depth, 1);
}

Vector ComputeRefraction(const Intersection& intersection, const Ray& prev_ray, const Scene& scene,
                                                                                const BVH& bvh,
                                                                                double env_refraction_indx,
                                                                                double mat_refraction_indx, 
                                                                                                int depth) {
    std::optional<Vector> refraction_dir = Refract(prev_ray.GetDirection(), intersection.GetNormal(), env_refraction_indx / mat_refraction_indx);
    if (!refraction_dir.has_value()) {
        return {0, 0, 0};
    }
    Ray refracted_ray{intersection.GetPosition(), refraction_dir.value()};
    return ComputeColorFull(refracted_ray, scene, bvh, depth, mat_refraction_indx);
}

std::pair<Vector, Vector> ComputeDiffusiveSpecular (const Scene& scene, const BVH& bvh, const Ray& prev_ray, 
                                                    const Intersection& intersection, double specular_exp) {
    Vector diffuse_component, specular_component, point_to_light;
    Vector point_intersection = intersection.GetPosition();
    Vector normal = intersection.GetNormal();
    for (auto light : scene.GetLights()) {
        point_to_light = light.position - point_intersection;
        auto point_to_light_len = Length(point_to_light);
        auto point_to_light_dir = UnitVector(point_to_light);
        auto light_check = bvh.Intersect({point_intersection, point_to_light_dir}).first;
        if (!light_check.has_value() || light_check->GetDistance() > point_to_light_len) {
            diffuse_component += light.intensity * ComputeDiffuse(point_to_light, normal); 
            specular_component += light.intensity * ComputeSpecular(point_to_light, normal, 
                                                                prev_ray.GetDirection(), specular_exp);
        }
    }
    return {diffuse_component, specular_component};
}

Vector ComputeColorNormal(const Ray& ray, const BVH& bvh) {
    auto [intersection, material] = bvh.Intersect(ray);
    if (intersection == std::nullopt) {
        return {0, 0, 0};
    }
    Vector normal = intersection->GetNormal();
    normal = normal * 0.5 + 0.5;
    return normal;
}

Vector ComputeColorDistance(const Ray& ray, const BVH& bvh) {
    auto [intersection, material] = bvh.Intersect(ray);
    if (intersection == std::nullopt) {
        return {0, 0, 0};
    }
    Vector distance(intersection->GetDistance());
    return distance;
}

Vector ComputeColorFull(const Ray& ray, const Scene& scene, const BVH& bvh, int depth, double refraction_index) {
    if (depth < 1) {
        return {0, 0, 0};
    }
    auto [intersection, material] = bvh.Intersect(ray);
    if (!intersection.has_value() || material == nullptr) {
        return {0, 0, 0};
    }
    Vector albedo = material->albedo;
    Vector diffuse_component, specular_component;
    if (albedo[0]) {
        std::tie(diffuse_component, specular_component)  = ComputeDiffusiveSpecular(scene, bvh, ray, intersection.value(), material->specular_exponent);
    }
    Vector phong_diff = material->diffuse_color * diffuse_component;
    Vector phong_spec = material->specular_color * specular_component;
    Vector reflection, refraction;
    if (albedo[1] && refraction_index == 1) {
        reflection = ComputeReflection(intersection.value(), ray, scene, bvh, depth - 1);
    }
    if (albedo[2]) {
        refraction = ComputeRefraction(intersection.value(), ray, scene, bvh, refraction_index, material->refraction_index, depth - 1);
    }
    return material->ambient_color + material->intensity + albedo[0] * (phong_diff + phong_spec) + albedo[1] * reflection + albedo[2] * refraction;
}

Image Render(const Camera& camera, const Scene& scene, const BVH& bvh, const RenderOptions& render_options, std::chrono::duration<double>* duration) {
    std::vector<std::vector<Vector>> image_buffer;
    Image image(camera.screen_width_, camera.screen_height_);
    image_buffer.resize(camera.screen_height_);
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t j = 0; j < camera.screen_height_; ++j) {
        for (size_t i = 0; i < camera.screen_width_; ++i) {
            Vector color;
            for (int sample = 0; sample < camera.samples_per_pixel_; ++sample) {
                auto direction = ComputeDirection(i, j, camera);
                Ray ray(camera.new_look_from_, direction);
                    switch (render_options.mode) {
                        case RenderMode::kDepth :
                            IncrementPrimaryRays();
                            color += ComputeColorDistance(ray, bvh);
                            break;
                        case RenderMode::kNormal :
                            IncrementPrimaryRays();
                            color += ComputeColorNormal(ray, bvh);
                            break;
                        case RenderMode::kFull :
                            IncrementPrimaryRays();
                            color += ComputeColorFull(ray, scene, bvh, render_options.depth, 1);
                            break;
                        default:
                            break;
                    }
            }
            color *= camera.sample_per_pixel_scale_;
            image_buffer[j].push_back(color);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    *duration = end - start; 
    double max = FindMax(image_buffer, camera);
    DivideTemplate(image_buffer, &image, camera, max);
    return image;
}