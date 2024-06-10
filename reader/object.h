#pragma once

#include "../geom/geometry.h"
#include "scene_utils.h"

class Object {

    public:

    virtual std::optional<Intersection> GetIntersection(const Ray& ray) const = 0; 

    const Material* GetMaterial() const {
        return material_;
    }

    virtual ~Object() {}

    virtual void PrintPrivateMembers() const = 0;

    private:

    const Material* material_;

};

class FaceObject : public Object  {

    public:

    FaceObject(const Material* mat ,std::array<Vector, 3> pt1, std::array<Vector, 3> pt2, std::array<Vector, 3> pt3) :
      material_(mat)
    , polygon_(pt1[0], pt2[0], pt3[0])
    , textures_(pt1[1], pt2[1], pt3[1])
    , normals_(pt1[2], pt2[2], pt3[2]) {}

    std::optional<Intersection> GetIntersection(const Ray& ray) const override {
        return GetTriangleIntersection(ray, polygon_);
    }

    const Vector* GetNormal(size_t index) const {
        return &normals_.GetVertex(index);
    }

    void PrintPrivateMembers() const override {
        std::cout << "\n\n FACE\n\n";
        std::cout << "Material: " << material_->name << std::endl;
        std::cout << "Triangle: "   << "\nPt1: ";
        PrintVec(polygon_.GetVertex(0));
        std::cout << "Pt2: ";
        PrintVec(polygon_.GetVertex(1));
        std::cout << "Pt3: ";
        PrintVec(polygon_.GetVertex(2));
        for (int i = 0; i < 3; ++i) {
                std::cout << "\tNormal for pt" << i << ": ";
                PrintVec(normals_.GetVertex(i));
                std::cout << "\n";
        }
    }

    private:

    const Material *material_;
    Triangle polygon_;
    Triangle textures_;
    Triangle normals_;
};

class SphereObject : public Object {

    public:

    SphereObject(const Material* material, const Sphere& sphere) : material_(material)
                                                                 , sphere_(sphere) {}

    std::optional<Intersection> GetIntersection(const Ray& ray) const override {
        return GetSphereIntersection(ray, sphere_);
    }

    void PrintPrivateMembers() const override {
        std::cout << "\n\n SPHERE\n\n";
        std::cout << "Material: " << material_->name << std::endl;;
        std::cout << "Sphere with center: ";
        PrintVec(sphere_.GetCenter());
        std::cout << " and radius " << sphere_.GetRadius() << "\n";
        std::cout << "\n";
    }

    private:

    const Material *material_;
    Sphere sphere_;
};
