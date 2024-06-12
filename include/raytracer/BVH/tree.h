#pragma once

#include "../common_libs.h"
#include "../logs.h"
#include "box.h"

struct OctNode {

    bool is_leaf_;
    std::array<std::unique_ptr<OctNode>, 8> children_;
    std::vector<std::unique_ptr<Object>> data_;
    size_t depth_of_node_;
    int index_;
    Box box_;

    OctNode() = default;

    OctNode(const Box& box) : is_leaf_(true) 
                            , depth_of_node_(0)
                            , index_(-1)
                            , box_(box)
    {}

    OctNode(int index, const Box& parentBox, size_t depth)
    : is_leaf_(true)
    , depth_of_node_(depth) 
    , index_(index) {
        Vector top(parentBox.GetCentroid());
        Vector bot(parentBox.GetCentroid());
        index & 4 ? top[2] = parentBox.GetTop()[2] : bot[2] = parentBox.GetBot()[2];
        index & 2 ? top[1] = parentBox.GetTop()[1] : bot[1] = parentBox.GetBot()[1];
        index & 1 ? top[0] = parentBox.GetTop()[0] : bot[0] = parentBox.GetBot()[0]; 
        box_ = Box(top, bot);  
        }

    ~OctNode() {}
};

 struct OctQueue {

    const OctNode* node_;
    double t_;

    OctQueue(const OctNode* node, double t) 
    : node_(node)
    , t_(t)
    {}

    bool operator > (const OctQueue& rhs) const {
        return t_ > rhs.t_;
    }

 };

class BVH {

    public:

    BVH(Scene& scene) {
        if (scene.GetNonConstObjects().empty()) {
            throw std::runtime_error("No objects at the scene");
            return ;
        }
        Box scene_box = scene.GetObjects()[0]->GetBox();
        for(auto& object : scene.GetObjects()) {
            scene_box.ExtendBox(object->GetBox());
        }
        root_ = std::make_unique<OctNode>(scene_box);
        for(auto& object : scene.GetNonConstObjects()) {
            InsertObject(root_, object, 0);
        }
        RecomputeBounds(root_);
    }

    const std::unique_ptr<OctNode>& GetRoot() const {
        return root_;
    }

    void InsertObject(std::unique_ptr<OctNode>& node, std::unique_ptr<Object>& object, size_t depth) {
        if (depth > depth_) {
            return ;
        }
        if (node->is_leaf_) {
            if (node->data_.empty() || depth == depth_) {
                 node->data_.push_back(std::move(object));
            } else {
                 node->is_leaf_ = false;
                 for (auto& elem : node->data_) {
                    InsertObject(node, elem, depth);
                }
                InsertObject(node, object, depth);
            }
        } else {
            int index = ComputeIndex(object->GetBox().GetCentroid(), node->box_.GetCentroid());
            if (node->children_[index] == nullptr) {
                node->children_[index] = std::make_unique<OctNode>(index, node->box_, depth + 1);
            }
            InsertObject(node->children_[index], object, depth + 1);
        }    
    }

    void RecomputeBounds(std::unique_ptr<OctNode>& node) {
        if (node->is_leaf_) {
            for (auto& elem : node->data_) {
                 node->box_.ExtendBox(elem->GetBox());
            } 
        } else {
            for (auto& child : node->children_) {
                if (child != nullptr) {
                     RecomputeBounds(child);
                     node->box_.ExtendBox(child->box_);
                } 
            }
        }
    }

    std::pair<std::optional<Intersection>, std::shared_ptr<Material>> Intersect(const Ray& ray) const {
        auto intersect_box = IntersectBox(ray, root_->box_);
        std::optional<Intersection> intersect_result = std::nullopt;
        double epsilon = 1e-6;
        std::shared_ptr<Material> material = nullptr;
        if (!intersect_box.has_value()) {
            return {std::nullopt, material};
        }
        auto [tNear, tFurther] = intersect_box.value();
        double tMin = tFurther;
        std::priority_queue<OctQueue, std::vector<OctQueue>, std::greater<OctQueue>> queue;
        queue.emplace(root_.get(), 0);
        while (!queue.empty() && (queue.top().t_ < tMin || std::fabs(tMin - queue.top().t_) <= epsilon)) {
            const OctNode* node = queue.top().node_;
            queue.pop();
            if (node->is_leaf_) {
                for (const auto& object : node->data_) {
                    auto intersect_obj = object->GetIntersection(ray);
                    if (intersect_obj.has_value()) {
                        double distance = intersect_obj->GetDistance();
                        if (distance < tMin || std::fabs(tMin - distance) <= epsilon){
                            IncrementRayObjectIntersect();
                            tMin = distance;
                            intersect_result = intersect_obj;
                            material = object->GetMaterial();
                        }
                    }
                }
            } else {
                for (const auto& child : node->children_) {
                    if (child != nullptr) {
                        intersect_box = IntersectBox(ray, child->box_);
                        if (intersect_box.has_value()) {
                            auto [tNearChild, tFarChild] = intersect_box.value();
                            double t = (tNearChild < 0 && tFarChild >= 0) ? tFarChild : tNearChild;
                            queue.emplace(child.get(), t);
                        } 
                    }
                }
            }
        }
        return {intersect_result, material};
    }

    ~BVH() {}

    private:

    int ComputeIndex(const Vector& object_centroid, const Vector& node_centroid) {
        int index = 0;
        if (object_centroid[2] >= node_centroid[2]) {
            index += 4;
        } 
        if (object_centroid[1] >= node_centroid[1]) {
            index += 2;
        } 
        if (object_centroid[0] >= node_centroid[0]) {
            index += 1;
        } 
        return index;
    }

    std::unique_ptr<OctNode> root_;
    size_t  depth_{15};
    
};

void PrintNode(const std::unique_ptr<OctNode>& node) {
    if (node == nullptr) {
            return ;
    }
    if (node->is_leaf_) {
        std::cout << "\t\t LEAF " << node << "\n\n";
        std::cout << "Node on depth " << node->depth_of_node_ << "\n\n";
        std::cout << "Index of node: " << node->index_ << "\n\n";
        std::cout << "###############\t ELEMENTS \t #################";
        for (const auto& elem : node->data_) {
            elem->PrintPrivateMembers();
        }
        std::cout << "###############################################\n\n";
    } else {
        std::cout << "Node on depth" << node->depth_of_node_ << "\n\n";
        std::cout << "Index of node: " << node->index_ << "\n\n";
        std::cout << "We'll be printing babies!\n" << std::endl;
        for (const auto& child : node->children_) {
            PrintNode(child);
        }
        
    }
}