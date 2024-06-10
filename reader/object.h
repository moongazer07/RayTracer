#pragma once


#include "../geom/geometry.h"
#include "scene_utils.h"
#include "../raytracer/BVH/box.h"

class Object {

    public:

    Object(const Material& material) : material_(std::make_shared<Material>(material)) {}

    virtual std::optional<Intersection> GetIntersection(const Ray& ray) const = 0;

    virtual ~Object() {}

    virtual void PrintPrivateMembers() const = 0;

    const std::shared_ptr<Material>& GetMaterial() const {
        return material_;
    };

    const Box& GetBox() const {
        return box_;
    }

    protected:

    Box box_;
    std::shared_ptr<Material> material_;

};

class FaceObject : public Object  {

    public:

    FaceObject(const Material& material, std::array<Vector, 3> pt1, std::array<Vector, 3> pt2, std::array<Vector, 3> pt3)
    : Object(material)
    , polygon_(pt1[0], pt2[0], pt3[0])
    , textures_(pt1[1], pt2[1], pt3[1])
    , normals_(pt1[2], pt2[2], pt3[2]) {
        box_ = Box(polygon_);
    }

    std::optional<Intersection> GetIntersection(const Ray& ray) const override {
        return GetTriangleIntersection(ray, polygon_);
    }

    const Vector* GetNormal(size_t index) const {
        return &normals_.GetVertex(index);
    }

    void PrintPrivateMembers() const override {
        std::cout << "\n\n FACE\n\n";
        std::cout << "Material: " << GetMaterial()->name << std::endl;
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
        std::cout << "Box:\n";
        std::cout << "Bottom: ";
        PrintVec(box_.GetBot());
        std::cout << "Top: ";
        PrintVec(box_.GetTop());
        std::cout << "Centroid: ";
        PrintVec(box_.GetCentroid());
    }

    private:

    Triangle polygon_;
    Triangle textures_;
    Triangle normals_;
};

class SphereObject : public Object {

    public:

    SphereObject(const Material& material, const Sphere& sphere)
    : Object(material)
    , sphere_(sphere) {
        box_ = Box(sphere_);
    }

    std::optional<Intersection> GetIntersection(const Ray& ray) const override {
        return GetSphereIntersection(ray, sphere_);
    }

    void PrintPrivateMembers() const override {
        std::cout << "\n\n SPHERE\n\n";
        std::cout << "Material: " << GetMaterial()->name << std::endl;;
        std::cout << "Sphere with center: ";
        PrintVec(sphere_.GetCenter());
        std::cout << " and radius " << sphere_.GetRadius() << "\n";
        std::cout << "\n\n";
        std::cout << "Box:\n";
        std::cout << "Bottom: ";
        PrintVec(box_.GetBot());
        std::cout << "Top: ";
        PrintVec(box_.GetTop());
        std::cout << "Centroid: ";
        PrintVec(box_.GetCentroid());
    }

    private:

    Sphere sphere_;
};
