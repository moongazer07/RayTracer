#include "scene.h"

int main () {
    Scene scene = ReadScene("cube.obj");
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
    for (auto element : scene.GetObjects()) {
        element->PrintPrivateMembers();
    }

    std::cout << "\n\t Lights \n\n";
    for (auto light : scene.GetLights()) {
        std::cout << "Added a light with position: ";
        PrintVec(light.position);
        std::cout << "   \t\tand intensivity ";
        PrintVec(light.intensity);
    }
}