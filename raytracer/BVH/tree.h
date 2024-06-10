
#include <memory>
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

    // OctNode(const OctNode&& other)
    //     : is_leaf_(other.is_leaf_)
    //     , children_(std::move(other.children_))
    //     , data_(std::move(other.data_))
    //     , depth_of_node_(other.depth_of_node_)
    //     , index_(other.index_)
    //     , box_(std::move(other.box_)) {}

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

    const OctNode* OctNode_;
    double t_;

    OctQueue(const std::unique_ptr<OctNode>& node, double t) 
    : OctNode_(node.get())
    , t_(t)
    {}

    bool operator < (const OctQueue& rhs) {
        return t_ < rhs.t_;
    }

 };

class BVH {

    public:

    BVH(Scene& scene) {
        if (scene.GetNonConstObjects().empty()) {
            std::cerr << "No objects in the scene" << std::endl;
            return ;
        }
        Box scene_box = scene.GetObjects()[0]->GetBox();
        for(auto& object : scene.GetObjects()) {
            scene_box.ExtendBox(object->GetBox());
        }
        root_ = std::make_unique<OctNode>(scene_box);
        std::cout << "Root address: " << root_ << std::endl;
        std::cout << "The scene_box:" << std::endl;
        std::cout << "Bot:";
        PrintVec(root_->box_.GetBot());
        std::cout << "Top: ";
        PrintVec(root_->box_.GetTop());
        std::cout << "Centroid: ";
        PrintVec(root_->box_.GetCentroid());
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
            std::cout << "The depth is too much!" << depth << std::endl;
            return ;
        }
        std::cout << "Input depth is " << depth << std::endl;
        if (node->is_leaf_) {
            std::cout << "Wa are in a leaf! " << node << std::endl;
            if (node->data_.empty() || depth == depth_) {
                std::cout << "Now added a new elemen in the leaf! " << node << " on depth " << depth <<  std::endl;
                 node->data_.push_back(std::move(object));
            } else {
                std::cout << "It was a fake leaf! " << depth << std::endl;
                 node->is_leaf_ = false;
                 for (auto& elem : node->data_) {
                    InsertObject(node, elem, depth);
                }
                InsertObject(node, object, depth);
            }
        } else {
            std::cout << "We will construct some babies of " << node << std::endl;
            int index = ComputeIndex(object->GetBox().GetCentroid(), node->box_.GetCentroid());
            std::cout << "Computed index! " << index << std::endl;
            if (node->children_[index] == nullptr) {
                std::cout << "A baby was null! " << std::endl;
                node->children_[index] = std::make_unique<OctNode>(index, node->box_, depth + 1);
                std::cout << "But now it ain't " << node->children_[index] << std::endl;
            }
            InsertObject(node->children_[index], object, depth + 1);
        }    
    }

    void RecomputeBounds(std::unique_ptr<OctNode>& node) {
        if (node->is_leaf_) {
            std::cout << "Old bounds of node " << node << std::endl;
            std::cout << "Bot:";
            PrintVec(node->box_.GetBot());
            std::cout << "Top: ";
            PrintVec(node->box_.GetTop());
            std::cout << "Centroid: ";
            PrintVec(node->box_.GetCentroid());
            for (auto& elem : node->data_) {
                 node->box_.ExtendBox(elem->GetBox());
            } 
            std::cout << "New bounds of node " << node << std::endl;
            std::cout << "Bot:";
            PrintVec(node->box_.GetBot());
            std::cout << "Top: ";
            PrintVec(node->box_.GetTop());
            std::cout << "Centroid: ";
            PrintVec(node->box_.GetCentroid());
        } else {
            std::cout << "Old bounds of node " << node << std::endl;
            std::cout << "Bot:";
            PrintVec(node->box_.GetBot());
            std::cout << "Top: ";
            PrintVec(node->box_.GetTop());
            std::cout << "Centroid: ";
            PrintVec(node->box_.GetCentroid());
            for (auto& child : node->children_) {
                if (child != nullptr) {
                     RecomputeBounds(child);
                     node->box_.ExtendBox(child->box_);
                } 
            }
            std::cout << "New bound of node " << node << std::endl;
            std::cout << "Bot:";
            PrintVec(node->box_.GetBot());
            std::cout << "Top: ";
            PrintVec(node->box_.GetTop());
            std::cout << "Centroid: ";
            PrintVec(node->box_.GetCentroid());
        }
    }

    std::optional<Intersection> Intersect(const Ray& ray) {
        auto intersect_box = IntersectBox(ray, root_.box_);
        std::optional<Intersection> intersect_result = std::null_opt;
        if (!intersect_box.has_value())
             return std::null_opt;
        auto [tNear, tFurther] = intersect_box.value();
        tMin = tFurther;
        std::priority_queue<OctQueue, std::vector<OctQueue>, std::greater<OctQueue>> queue;
        queue.push(OctQueue(t, root_));
        while (!queue.empty() && queue.top().t_ < tMin) {
            const OctQueue& node = queue.top();
            queue.pop();
            if (node.is_leaf_) {
                for (const auto& object : node->data_) {
                    auto intersect_obj = CheckIntersectiont(ray, object);
                    if (intesect.has_value() && intersect->dist_ < tMin) {
                        tMin = intersect->dist_();
                        intersect_result = intersect_obj;
                    }
                }
            } else {
                for (const auto& child : node->children_) {
                    if (child != nullptr) {
                        intersect_box = IntersectBox(ray, child->box_);
                        if (intersect_box.has_value()) {
                            auto [tNearChild, tFarChild] = intersect_box.value();
                            double t = (tNearChild < 0 && tFarChild > 0) ? tFarChild : tNearChild;
                            queue.push(OctQueue(t, child));
                        } 
                    }
                }
            }
        }
        // return intrsect_result;
        return std::make_opt({0, 0, 0}, {0, 1, 0}, tMin);
    }

    ~BVH() {}

    private:

    int ComputeIndex(const Vector& object_centroid, const Vector& node_centroid) {
        int index = 0;
        std::cout << "Here are our centroids:\n";
        std::cout << "Object centroid: ";
        PrintVec(object_centroid);
        std::cout << "Node centroid: ";
        PrintVec(node_centroid); 
        if (object_centroid[2] > node_centroid[2]) {
            index += 4;
        } 
        if (object_centroid[1] > node_centroid[1]) {
            index += 2;
        } 
        if (object_centroid[0] > node_centroid[0]) {
            index += 1;
        } 
        std::cout << "The index function gives: " << index << std::endl;
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