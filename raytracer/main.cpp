#include "raytracerlib.h"
#include "camera.h"

// main gets as an argument the name of the .OBJ file
// Then it retreives the scene from the file (parsing part)
// And the second step is rendering itself
int main () {
    std::string name_of_file = "/home/kkopnev/Documents/RayTracer/raytracer/tests/mirrors/scene.obj";
    // std::string name_of_file = "C:/Users/kerel/my_code/C++/RayTracer/raytracer/tests/one_sphere.obj";

    size_t image_width = 600;
    size_t image_height = 480;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    Scene scene = ReadScene(name_of_file);
    Camera camera{image_width, image_height, 90, {2.5, 1.5, -0.1}, {1, 1.2, -2.8}};
    RenderOptions options{9, RenderMode::kNormal};
    Render(camera, scene, options);
}