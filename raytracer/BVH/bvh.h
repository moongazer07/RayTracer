class BVH : public AccelerationStructure
{
    static const uint8_t kNumPlaneSetNormals = 7;
    static const Vec3f planeSetNormals[kNumPlaneSetNormals];
    struct Extents
    {
        Extents()
        {
            for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i)
                d[i][0] = kInfinity, d[i][1] = -kInfinity;
        }
        void extendBy(const Extents &extents)
        {
            for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i) {
                if (extents.d[i][0] < d[i][0]) d[i][0] = extents.d[i][0];
                if (extents.d[i][1] > d[i][1]) d[i][1] = extents.d[i][1];
            }
        }
        bool intersect(
            const float *precomputedNumerator, const float *precomputeDenominator, 
            float &tNear, float &tFar, uint8_t &planeIndex);
        float d[kNumPlaneSetNormals][2]; // d values for each plane-set normals
        const Object *object; // pointer contained by the volume (used by octree)
    };
    Extents *extents;
    struct OctreeNode
    {
        OctreeNode *child[8];
        std::vector<const Extents *> data;
        Extents extents;
        bool isLeaf;
        uint8_t depth; // just for debugging
        OctreeNode() : isLeaf(true) { memset(child, 0x0, sizeof(OctreeNode *) * 8); }
        ~OctreeNode() { for (uint8_t i = 0; i < 8; ++i) if (child[i] != NULL) delete child[i]; }
    };
    struct Octree
    {
        Octree(const Extents &extents) : root(NULL)
        {
            float xdiff = extents.d[0][1] - extents.d[0][0];
            float ydiff = extents.d[1][1] - extents.d[1][0];
            float zdiff = extents.d[2][1] - extents.d[2][0];
            float dim = std::max(xdiff, std::max(ydiff, zdiff));
            Vec3f centroid(
                (extents.d[0][0] + extents.d[0][1]),
                (extents.d[1][0] + extents.d[1][1]),
                (extents.d[2][0] + extents.d[2][1]));
            bounds[0] = (Vec3f(centroid) - Vec3f(dim)) * 0.5f;
            bounds[1] = (Vec3f(centroid) + Vec3f(dim)) * 0.5f;
            root = new OctreeNode;
        }
        void insert(const Extents *extents)
        { insert(root, extents, bounds[0], bounds[1], 0); }
        void build()
        { build(root, bounds[0], bounds[1]); }
        ~Octree() { delete root; }
        struct QueueElement
        {
            const OctreeNode *node; // octree node held by this node in the tree
            float t; // used as key
            QueueElement(const OctreeNode *n, float thit) : node(n), t(thit) {}
            // comparator is > instead of < so priority_queue behaves like a min-heap
            friend bool operator < (const QueueElement &a, const QueueElement &b) { return a.t > b.t; }
        };
        Vec3f bounds[2];
        OctreeNode *root;
    private:
        void insert(
            OctreeNode *node, const Extents *extents, 
            Vec3f boundMin, Vec3f boundMax, int depth)
        {
            if (node->isLeaf) {
                if (node->data.size() == 0 || depth == 16) {
                    node->data.push_back(extents);
                }
                else {
                    node->isLeaf = false;
                    while (node->data.size()) {
                        insert(node, node->data.back(), boundMin, boundMax, depth);
                        node->data.pop_back();
                    }
                    insert(node, extents, boundMin, boundMax, depth);
                }
            } else {
                // insert bounding volume in the right octree cell
                Vec3f extentsCentroid = (
                    Vec3f(extents->d[0][0], extents->d[1][0], extents->d[2][0]) +
                    Vec3f(extents->d[0][1], extents->d[1][1], extents->d[2][1])) * 0.5;
                Vec3f nodeCentroid = (boundMax + boundMin) * 0.5f;
                uint8_t childIndex = 0;
                if (extentsCentroid[0] > nodeCentroid[0]) childIndex += 4;
                if (extentsCentroid[1] > nodeCentroid[1]) childIndex += 2;
                if (extentsCentroid[2] > nodeCentroid[2]) childIndex += 1;
                Vec3f childBoundMin, childBoundMax;
                Vec3f boundCentroid = (boundMin + boundMax) * 0.5;
                computeChildBound(childIndex, boundCentroid, boundMin, boundMax, childBoundMin, childBoundMax);
                if (node->child[childIndex] == NULL)
                      node->child[childIndex] = new OctreeNode, node->child[childIndex]->depth = depth;
                insert(node->child[childIndex], extents, childBoundMin, childBoundMax, depth + 1);
            }
        }
        void computeChildBound(
            const uint8_t &i, const Vec3f &boundCentroid,
            const Vec3f &boundMin, const Vec3f &boundMax,
            Vec3f &pMin, Vec3f &pMax) const
        {
            pMin[0] = (i & 4) ? boundCentroid[0] : boundMin[0];
            pMax[0] = (i & 4) ? boundMax[0] : boundCentroid[0];
            pMin[1] = (i & 2) ? boundCentroid[1] : boundMin[1];
            pMax[1] = (i & 2) ? boundMax[1] : boundCentroid[1];
            pMin[2] = (i & 1) ? boundCentroid[2] : boundMin[2];
            pMax[2] = (i & 1) ? boundMax[2] : boundCentroid[2];    
        }
        // bottom-up construction
        void build(OctreeNode *node, const Vec3f &boundMin, const Vec3f &boundMax)
        {
            if (node->isLeaf) {
                // compute leaf node bounding volume
                for (uint32_t i = 0; i < node->data.size(); ++i) {
                    node->extents.extendBy(*node->data[i]);
                }
            }
            else {
                for (uint8_t i = 0; i < 8; ++i)
                    if (node->child[i]) {
                        Vec3f childBoundMin, childBoundMax;
                        Vec3f boundCentroid = (boundMin + boundMax) * 0.5;
                        computeChildBound(i, boundCentroid, boundMin, boundMax, childBoundMin, childBoundMax);
                        build(node->child[i], childBoundMin, childBoundMax);
                        node->extents.extendBy(node->child[i]->extents);
                    }
                }
            }
        }
    };
    Octree *octree;
public:
    BVH(const RenderContext *rcx);
    const Object* intersect(const Ray<float> &ray, IsectData &isectData) const;
    ~BVH();
};

BVH::BVH(const RenderContext *rcx) : AccelerationStructure(rcx), extents(NULL), octree(NULL) 
{ 
    Extents sceneExtents; 
    extents = new Extents[rcx->objects.size()]; 
    for (uint32_t i = 0; i < rcx->objects.size(); ++i) { 
        for (uint8_t j = 0; j < kNumPlaneSetNormals; ++j) { 
            rcx->objects[i]->computeBounds(planeSetNormals[j], extents[i].d[j][0], extents[i].d[j][1]); 
        } 
        extents[i].object = rcx->objects[i]; 
        sceneExtents.extendBy(extents[i]); 
    } 
    // create hierarchy                                                                                                                                                                                     
    octree = new Octree(sceneExtents); 
    for (uint32_t i = 0; i < rcx->objects.size(); ++i) { 
        octree->insert(extents + i); 
    } 
    octree->build(); 
} 
 
inline bool BVH::Extents::intersect( 
    const float *precomputedNumerator, const float *precomputeDenominator, 
    float &tNear, float &tFar, uint8_t &planeIndex) 
{ 
    __sync_fetch_and_add(&numRayVolumeTests, 1); 
    for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i) { 
        float tn = (d[i][0] - precomputedNumerator[i]) / precomputeDenominator[i]; 
        float tf = (d[i][1] - precomputedNumerator[i]) / precomputeDenominator[i]; 
        if (precomputeDenominator[i] < 0) std::swap(tn, tf); 
        if (tn > tNear) tNear = tn, planeIndex = i; 
        if (tf < tFar) tFar = tf; 
        if (tNear > tFar) return false;  //test for an early stop 
    } 

    return true; 
} 
 
const Object* BVH::intersect(const Ray<float> &ray, IsectData &isectData) const 
{ 
    const Object *hitObject = NULL; 
    float precomputedNumerator[BVH::kNumPlaneSetNormals], precomputeDenominator[BVH::kNumPlaneSetNormals]; 
    for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i) { 
        precomputedNumerator[i] = dot(planeSetNormals[i], ray.orig); 
        precomputeDenominator[i] = dot(planeSetNormals[i], ray.dir);; 
    } 
#if 0 
    float tClosest = ray.tmax; 
    for (uint32_t i = 0; i < rc->objects.size(); ++i) { 
        __sync_fetch_and_add(&numRayVolumeTests, 1); 
        float tNear = -kInfinity, tFar = kInfinity; 
        uint8_t planeIndex; 
        if (extents[i].intersect(precomputedNumerator, precomputeDenominator, tNear, tFar, planeIndex)) { 
            IsectData isectDataCurrent; 
            if (rc->objects[i]->intersect(ray, isectDataCurrent)) { 
                if (isectDataCurrent.t < tClosest && isectDataCurrent.t > ray.tmin) { 
                    isectData = isectDataCurrent; 
                    hitObject = rc->objects[i]; 
                    tClosest = isectDataCurrent.t; 
                } 
            } 
        } 
    } 
#else 
    uint8_t planeIndex = 0; 
    float tNear = 0, tFar = ray.tmax; 
    if (!octree->root->extents.intersect(precomputedNumerator, precomputeDenominator, tNear, tFar, planeIndex) 
        || tFar < 0 || tNear > ray.tmax) 
        return NULL; 
    float tMin = tFar; 
    std::priority_queue<BVH::Octree::QueueElement> queue; 
    queue.push(BVH::Octree::QueueElement(octree->root, 0)); 
    while(!queue.empty() && queue.top().t < tMin) { 
        const OctreeNode *node = queue.top().node; 
        queue.pop(); 
        if (node->isLeaf) { 
            for (uint32_t i = 0; i < node->data.size(); ++i) { 
                IsectData isectDataCurrent; 
                if (node->data[i]->object->intersect(ray, isectDataCurrent)) { 
                    if (isectDataCurrent.t < tMin) { 
                        tMin = isectDataCurrent.t; 
                        hitObject = node->data[i]->object; 
                        isectData = isectDataCurrent; 
                    } 
                } 
            } 
        } 
        else { 
            for (uint8_t i = 0; i < 8; ++i) { 
                if (node->child[i] != NULL) { 
                    float tNearChild = 0, tFarChild = tFar; 
                    if (node->child[i]->extents.intersect(precomputedNumerator, precomputeDenominator, 
                        tNearChild, tFarChild, planeIndex)) { 
                        float t = (tNearChild < 0 && tFarChild >= 0) ? tFarChild : tNearChild; 
                        queue.push(BVH::Octree::QueueElement(node->child[i], t)); 
                    } 
                } 
            } 
        } 
    } 
#endif 

    return hitObject; 
}