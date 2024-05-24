#include "raytracerlib.h"
#include "camera.h"

// main gets as an argument the name of the .OBJ file
// Then it retreives the scene from the file (parsing part)
// And the second step is rendering itself

int main () {
    std::string name_of_file = "/home/kkopnev/Documents/RayTracer/raytracer/tests/box/cube.obj";
    int image_width = 1080;
    int image_height = 720;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    Scene scene = ReadScene(name_of_file);
    Camera camera{image_width, image_height, 90, {0, 0, 0}, {0, 0, -1}};
    RenderOptions options{1, RenderMode::kNormal};
    // Image image = Render(camera, scene, options);
    Render(camera, scene, options);
    // image.Write("CornellBox.png");
}