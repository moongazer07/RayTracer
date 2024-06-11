#include "common_libs.h"
#include "camera.h"
#include "BVH/tree.h"
#include "render.h"
#include "renderBVH.h"

int main () {
    std::string name_of_file = "/home/kkopnev/Documents/RayTracer/tests/box/cube.obj";
    // std::string name_of_file = "C:/Users/kerel/my_code/C++/RayTracer/raytracer/tests/one_sphere.obj";

    size_t image_width = 640;
    size_t image_height = 480;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    Scene scene = ReadScene(name_of_file);
    BVH bvh = BVH(scene);
    Camera camera{image_width, image_height, 90, {2, 1.5, -0.1}, {1, 1.2, -2.8}};
    RenderOptions options{3, RenderMode::kFull};
    Render(camera, scene, bvh, options);
}