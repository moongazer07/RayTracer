
#include "box.h"

struct OctNode {

    bool leaf_;
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
        // depending on the indext
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

class OctTree {

    public:

    OctTree(const Box& scene_box) : root_(new OctNode(scene_box)) 
    {}

    // void InsertObject(Object) {
        // all the long process of insertion
    //}

    // void RecomputeBounds() {
        // if leaf --> extend bounds for every child
        // if not --> recursively do for every child
        //              extend by all the children 
    // }

    ~OctTree() {
        delete root_;
    }

    private:

    OctNode* root_;
    size_t  depth_{16};
    
};