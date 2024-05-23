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
    std::cout << "\n\t SPHERES \n\n";
    for (auto element : scene.GetSphereObjects()) {
        Sphere sph = element.sphere;
        auto mat = element.material;
        std::cout << "Material: " << mat->name << std::endl;;
        std::cout << "Sphere with center: ";
        PrintVec(sph.GetCenter());
        std::cout << " and radius " << sph.GetRadius() << "\n";
        std::cout << "\n";
    }
    std::cout << "\n\t FACES \n\n";
    for (auto element : scene.GetObjects()) {
        Triangle triangle = element.polygon;
        auto mat = element.material;
        std::cout << "Material: " << mat->name << std::endl;
        std::cout << "Triangle: "   << "\nPt1: ";
        PrintVec(triangle.GetVertex(0));
        std::cout << "Pt2: ";
        PrintVec(triangle.GetVertex(1));
        std::cout << "Pt3: ";
        PrintVec(triangle.GetVertex(2));
        for (int i = 0; i < 3; ++i) {
            Vector vec = *element.GetNormal(i);
                std::cout << "\tNormal for pt" << i << ": ";
                PrintVec(vec);
                std::cout << "\n";
        }
    }
    std::cout << "\n\t Lights \n\n";
    for (auto light : scene.GetLights()) {
        std::cout << "Added a light with position: ";
        PrintVec(light.position);
        std::cout << "   \t\tand intensivity ";
        PrintVec(light.intensity);
    }
}