#include "include/raytracer/common_libs.h"
#include "include/raytracer/logs.h"
#include "include/raytracer/camera.h"
#include "include/raytracer/BVH/tree.h"
#include "include/raytracer/render.h"
#include "include/raytracer/renderBVH.h"
#include "include/raytracer/image.h"

int main (int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error: expected input: <file_path> oprional:-BVH" << std::endl;
        return 1;
    }
    std::string name_of_file = std::string(argv[1]);
    size_t image_width = 144;
    size_t image_height = 144;
    double field_of_view = 90;
    const Vector LookFrom{0, 0, 0};
    const Vector LookTo{0, -1, 0};

    bool useBVH = false;
    if (argc > 2 && std::string(argv[2]) == "-BVH") {
        useBVH = true;
    }
    Scene scene;
    try {
        scene = ReadScene(name_of_file);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    Camera camera{image_width, image_height, field_of_view, LookFrom, LookTo};
    RenderOptions options{1, RenderMode::kNormal};
    std::chrono::duration<double> duration;
    try {
        if (useBVH) {
            BVH bvh = BVH(scene);
            Image image = Render(camera, scene, bvh, options, &duration);
            image.Write("../../../pictures/" + scene.GetOutputName());
        } else {
            Image image = Render(camera, scene, options, &duration);
            image.Write("../../../pictures/" + scene.GetOutputName());
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    try {
        WriteLogs(scene.GetOutputName(), useBVH, camera, options, duration);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } 
}