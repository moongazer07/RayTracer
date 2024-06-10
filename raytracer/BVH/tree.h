
#include "box.h"

struct OctNode {

    bool is_leaf_;
    std::array<OctNode*, 8> children_;
    // std::vector<Object> data;
    size_t depth_of_node_;
    Box box_;

    OctNode(const Box& box) : leaf_(true) 
                            , box_(box)
    {
        for (auto elem : children_) {
            elem = nullptr;
        }
    }

    OctNode(const uint8_t index, const Box& parentBox) : leaf_(true) {
        for (auto elem : children_) {
            elem = nullptr;
        }
        Vector top(parentBox.centroid_);
        Vector bot(parentBox.centroid_);
        // index & 4 ? top[2] = parentBox.top_[2] : bot[2] = parentBox.bot_[2];
        // index & 2 ? top[1] = parentBox.top_[1] : bot[1] = parentBox.bot_[1];
        // index & 1 ? top[0] = parentBox.top[0] : bot[0] = parentBox.bot[0]; 
        // box_ = Box(top, bot);  
        // }

        }
        // mid_mid
        // depending on the index
        // we will provide the bounds for the
        // Box
    }

    ~OctNode() {
        for (auto elem : children_) {
            if (elem != nullptr) {
                delete elem;
            }
        }
    }

};

 struct OctQueue {

    const OctNode& OctNode_;
    double t_;

    OctQueue(const OctNode& node, double t) : OctNode(node)
                                            , t_(t)
    {}

    bool operator < (const OctQueue& rhs) {
        return t_ < rhs.t_;
    }

 }

class BVH {

    public:

    BVH(const Box& scene_box) : root_(new OctNode(scene_box)) 
    {
        // Octree* octree = new Octree(scene.box);
        // for every object in the scene
        //      octree->insert(object);
        // RecomputeBounds(octree->root);
    }

    void InsertObject(const OctNode& node, const Object& object, size_t depth) {
        
        if (node.is_leaf) {
            // if (node.data_.empty() || depth = depth_) {
            //      node.data_.push_back(object);
            // } else {
            //      node.is_leaf_ = false;
            //      for (auto elem : node.data_) {
            //          insert(node, elem, depth)
            //      insert(node, object, depth) 
        } else {
            // uint8_t index = ComputeIndex(object->Box.centroid, node.centroid);
            // if (node.children_[index] == nullptr) {
            //      node.children_[index] = new OctNode(index, node.box_) 
            // }
            // insert(node.choldren_[index], object, depth + 1);
        }    
    }

    // void RecomputeBounds(OctNode* node) {
        // if (node.is_leaf_) {
            // for (auto elem : node.data_) {
            //      node->box_.ExtendTheBox(elem.Box) 
            // } 
        // else {
        //      for (auto child : children) {
            //      if (child != nullptr)
            //          RecomputeBounds(child);
            //          node->box_.ExtendTheBox(child.Box) 
        // }
        // }
    // }

    std::optional<Intersection> IntersectBVH(const Ray& ray) {
        // auto intersect_box = IntersectBox(ray, root_.box_);
        // std::optional<Intersection> intersect_result = std::null_opt;
        // if (!intersect_box.has_value())
        //      return std::null_opt;
        // auto [tNear, tFurther] = intersect_box.value();
        // tMin = tFurther;
        // std::priority_queue<double, std::vector<double>, std::greater<OctQueue>> queue;
        // queue.push(QueueNode(t, root));
        // while (!queue && queue.top.t_ < tMin) {
            // OctNode* node = queue.top();
            // queue.pop();
            // if (node.is_leaf_) {
            //      for (auto object : node.data_) {
            //          auto intersect_obj = CheckIntersectiont(ray, object);
            //      if (intesect.has_value() && intersect->dist_ < tMin) {
            //          tMin = intersect->dist_();
            //          intersect_result = intersect_obj;
            //      }
            // } else {
                // for (children) {
                //  if (child != nullptr) {
                    //  intersect_box = IntersectBox(ray, child.box_);
                //      if (intersect_box.has_value()) {
                //          auto [tNearChild, tFarChild] = intersect_box.value();
                //          double t = (tNearChild < 0 && tFarChild > 0) ? tFarChild : tNearChild;
                //          queue.push(QueueNode(t, child));
                //       } 
                // }
                // }
            // }
            // 
        // }
        // return intrsect_result;
    }

    ~BVH() {
        delete root_;
    }

    private:

    uint8_t ComputeIndex(const Vector& object_centroid, const Vector& node_centroid) {
        uint8_t index = 0;
        if (object_centroid[2] > node_centroid[2]) {
            index += 4;
        } 
        if (object_centroid[1] > node_centroid[1]) {
            index += 2;
        } 
        if (object_centroid[0] > node_centroid[0]) {
            index += 1;
        } 
        return index;
    }

    OctNode* root_;
    size_t  depth_{16};
    
};