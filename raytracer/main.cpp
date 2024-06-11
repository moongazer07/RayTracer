#include "raytracerlib.h"
#include "camera.h"
#include "BVH/tree.h"
#include "render.h"
#include "renderBVH.h"

// main gets as an argument the name of the .OBJ file
// Then it retreives the scene from the file (parsing part)
// And the second step is rendering itself

int main () {
    // std::string name_of_file = "/home/kkopnev/Documents/RayTracer/raytracer/tests/one_sphere.obj";
    std::string name_of_file = "C:/Users/kerel/my_code/C++/RayTracer/raytracer/tests/one_sphere.obj";

    size_t image_width = 640;
    size_t image_height = 480;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    Scene scene = ReadScene(name_of_file);
    // for (const auto& element : scene.GetObjects()) {
    //     element->PrintPrivateMembers();
    // }
    BVH bvh = BVH(scene);
    // PrintNode(bvh.GetRoot());
    Camera camera{image_width, image_height, 90, {-0.9, 1.9, -1}, {0.0, 0.0, 0}};
    RenderOptions options{3, RenderMode::kDepth};
    Render(camera, scene, bvh, options);
}