#pragma once

// #include <material.h>
// #include <vector.h>
// #include <object.h>
// #include <light.h>

#include "scene_utils.h"

class Scene {
public:

    Scene() = default;

    Scene(std::vector<Object> faces, std::vector<SphereObject> spheres,
            std::vector<Light> lights, std::map<std::string, Material> materials) 
            : Faces_(faces)
            , Spheres_(spheres)
            , Lights_(lights)
            , Materials_(materials) {};
            

    const std::vector<Object>& GetObjects() const {
        return Faces_;
    }

    const std::vector<SphereObject>& GetSphereObjects() const {
        return Spheres_;
    }
    const std::vector<Light>& GetLights() const {
        return Lights_;
    }

    const std::map<std::string, Material>& GetMaterials() const {
        return Materials_;
    }

    const std::vector<Vector> GetVertices() const {
        return Vertices_;
    }

    const std::vector<Vector> GetVectorNormals() const {
        return VectorNormals_;
    }

    const std::vector<Vector> GetVectorTextures() const {
        return VectorTextures_;
    }

    void SetMaterials(const std::map<std::string, Material>& materials) {
        Materials_ = materials;
    }

    void AddObject(Object obj) {
        Faces_.push_back(obj);
    }

    void AddSphere(SphereObject sphere) {
        Spheres_.push_back(sphere);
    }

    void AddLight(Light bulb) {
        Lights_.push_back(bulb);
    }

    void AddMaterial(std::string name , Material material) {
        Materials_.insert({name, material});
    }

    void AddVertex(Vector vec) {
        Vertices_.push_back(vec);
    }

    void AddVectorNormal(Vector vec) {
        VectorNormals_.push_back(vec);
    }

    void AddVectorTextures(Vector vec) {
        VectorTextures_.push_back(vec);
    }


private:
    std::vector<Vector> Vertices_;
    std::vector<Vector> VectorNormals_;
    std::vector<Vector> VectorTextures_;
    std::vector<Object> Faces_;
    std::vector<SphereObject> Spheres_;
    std::vector<Light> Lights_;
    std::map<std::string, Material> Materials_;
};

std::array<Vector, 3> FillVertex(Scene* scene, const std::array<int, 3>& index) {
    std::array<Vector, 3> vertex;

    if (static_cast<size_t>(index[0]) < scene->GetVertices().size()) {
        vertex.at(0) = scene->GetVertices().at(index[0]);
    }
    if (static_cast<size_t>(index[1]) < scene->GetVectorTextures().size()) {
        vertex.at(1) = scene->GetVectorTextures().at(index[1]);
    }
    if (static_cast<size_t>(index[2]) < scene->GetVectorNormals().size()) {
        vertex.at(2) = scene->GetVectorNormals().at(index[2]);
    }
    return vertex;
}

void RetreiveFaces(const std::vector<std::string>& splitted, const Material* material, Scene* scene) {
    std::array<Vector, 3> initial;
    std::array<Vector, 3> second;
    std::array<Vector, 3> third;
    std::array<int, 3> index;
    int retreived_vrtx = 0;

    for (size_t i = 1; i < splitted.size(); ++i) {
        if (splitted[i].empty()) {
            continue;
        }
        index = GetIndex(splitted[i], {scene->GetVertices().size(), scene->GetVectorTextures().size(), scene->GetVectorNormals().size()});
        if (i != 1 && i % 2 == 0) {
            second = FillVertex(scene, index);
            ++retreived_vrtx;
        } else if (i != 1 && i % 2 == 1) {
            third = FillVertex(scene, index);
            ++retreived_vrtx;
        } else if (i == 1 && !splitted[i].empty()) {
            initial = FillVertex(scene, index);
            ++retreived_vrtx;
        } 
        if (retreived_vrtx == 3) {
            scene->AddObject({material, initial, second, third});
            retreived_vrtx = 1;
        }
    }
}

inline std::map<std::string, Material> ReadMaterials(std::string_view filename) {
    std::string file_name{filename};

    std::ifstream mtl_file(file_name);

    if (!mtl_file.is_open()) {
        std::cerr << "Error opening the file" << std::endl;
    }
    std::string line;
    std::vector<std::string> splitted;
    std::map<std::string, Material> materials;
    bool new_mtl = false;
    Material current_material;
    size_t ind = 0;

    while(std::getline(mtl_file, line)) {
        ChangeToSpace(&line);
        splitted = Split(line, ' ');
        if (splitted.empty()) {
            continue;
        }
        splitted[0].empty() ? ind = FindNonEmpty(splitted, 0) : ind = 0;
        if (splitted[ind] == "newmtl") {
            if (new_mtl) { 
                materials.insert({current_material.name, current_material});
                current_material = Material();
            }
            new_mtl = true;
            current_material.name = splitted[FindNonEmpty(splitted, ind)];
        } else {
            FillMaterial(splitted, &current_material, &ind);
        }
    }
    if (new_mtl) {
        materials.insert({current_material.name, current_material});
    }
    mtl_file.close();
    return materials;
}

inline Scene ReadScene(std::string_view filename) { 
    std::string file_name{filename};

    std::ifstream obj_file(file_name);
    std::vector<std::string> splitted;

    if (!obj_file.is_open()) {
        std::cerr << "Error opening the file" << std::endl;
    }
    std::string line;
    Scene scene;
    const Material* current_material = nullptr;
    size_t ind = 0;

    while(std::getline(obj_file, line)) {
        ChangeToSpace(&line);
        splitted = Split(line, ' ');
        if (splitted.empty()) {
            continue;
        }
        splitted[0].empty() ? ind = FindNonEmpty(splitted, 0) : ind = 0;
        if (splitted[ind] == "v") {
            scene.AddVertex(RetreiveVector(splitted, &ind));
        } else if (splitted[ind] == "vf") {
            scene.AddVectorTextures(RetreiveVector(splitted, &ind));
        } else if (splitted[ind] == "vn") {
            scene.AddVectorNormal(RetreiveVector(splitted, &ind));
        } else if (splitted[ind] == "S") {
            scene.AddSphere({current_material, RetreiveSphere(splitted, &ind)});
        } else if (splitted[ind] == "P") {
            scene.AddLight(RetreiveLight(splitted, &ind));
        } else if (splitted[ind] == "f") {
            RetreiveFaces(splitted, current_material, &scene);
        } else if (splitted[ind] == "usemtl") {
                current_material = &scene.GetMaterials().at(splitted[FindNonEmpty(splitted, ind)]);
        } else if (splitted[ind] == "mtllib") {
            scene.SetMaterials(ReadMaterials(splitted[FindNonEmpty(splitted, ind)]));
        } 
    }
    obj_file.close();

    return scene;
}
