#include "raytracerlib.h"
#include "camera.h"

// main gets as an argument the name of the .OBJ file
// Then it retreives the scene from the file (parsing part)
// And the second step is rendering itself

int main () {
    // std::string name_of_file = "/home/kkopnev/Documents/RayTracer/raytracer/tests/distorted_box/CornellBox-Original.obj";
    std::string name_of_file = "C:/Users/kerel/my_code/C++/RayTracer/raytracer/tests/triangle/scene.obj";

    size_t image_width = 1080;
    size_t image_height = 1080;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    Scene scene = ReadScene(name_of_file);
    for (auto [key, material] : scene.GetMaterials()) {
        std::cout << "Material " << key << std::endl;
        std::cout << "Ka: ";
        PrintVec(material.ambient_color);
        std::cout << "Kd: ";
        PrintVec(material.diffuse_color);
        std::cout << "Ks: ";
        PrintVec(material.specular_color);
        std::cout << "Ke: ";
        PrintVec(material.intensity);
        std::cout << "Ns: " << material.specular_exponent << std::endl;
        std::cout << "Ni: " << material.refraction_index << std::endl;
        std::cout << "al: ";
        PrintVec(material.albedo);
        std::cout << "\n";
    }
    std::cout << "\n\t  \n\n";
    for (const auto& element : scene.GetObjects()) {
        element->PrintPrivateMembers();
    }

    std::cout << "\n\t Lights \n\n";
    for (auto light : scene.GetLights()) {
        std::cout << "Added a light with position: ";
        PrintVec(light.position);
        std::cout << "   \t\tand intensivity ";
        PrintVec(light.intensity);
    }
    // Camera camera{image_width, image_height, 90, {-0.5, 1.5, 1.98}, {0.0, 1.0, 0.0}};
    // RenderOptions options{9, RenderMode::kFull};
    // Render(camera, scene, options);
}