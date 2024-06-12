#pragma once
#include "common_libs.h"

inline void IncrementObjects();
inline void IncrementRayObjectTest();
inline void IncrementRayBoxTests();

#include "camera.h"
#include "render_options.h"

inline size_t numObjects = 0;
inline size_t numPrimaryRays = 0;
inline size_t numRayObjectTest = 0;
inline size_t numRayObjectIntersect = 0;
inline size_t numRayBoxTests = 0;

inline void IncrementObjects() {
    ++numObjects;
}

inline void IncrementPrimaryRays() {
    ++numPrimaryRays;
}

inline void IncrementRayObjectTest() {
    ++numRayObjectTest;
}

inline void IncrementRayObjectIntersect() {
    ++numRayObjectIntersect;
}

inline void IncrementRayBoxTests() {
    ++numRayBoxTests;
}

void WriteLogs(const std::string& outputName, bool useBVH, const Camera& camera, const RenderOptions& options , const std::chrono::duration<double>& duration) {
    std::ofstream logFile("../log.txt", std::ios::app);

    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file");
        return ;
    }

    logFile << "FILE: " << outputName << std::endl;
    logFile << "Image width: " << camera.screen_height_ << std::endl;
    logFile << "Image height: " << camera.screen_width_ << std::endl;
    logFile << "Number of objects: " << numObjects << std::endl;
    logFile << "Number of primary rays: " << numPrimaryRays << std::endl;
    logFile << "Total number of ray object tests: " << numRayObjectTest << std::endl;
    logFile << "Total number of ray object intersects: " << numRayObjectIntersect << std::endl;
    logFile << "Total number of ray box test: " << numRayBoxTests << std::endl;
    logFile << "Samples per pixel done: " << camera.samples_per_pixel_ << std::endl;
    logFile << "Duration of rendering: " << duration.count() << " seconds" << std::endl;
    logFile << "Rended mode: ";
    if (options.mode == RenderMode::kDepth) {
        logFile << "kDepth" << std::endl;
    } else if (options.mode == RenderMode::kFull) {
        logFile << "kFull with depth " << options.depth << std::endl;
    } else if (options.mode == RenderMode::kNormal) {
        logFile << "kNormal" << std::endl;
    }
    logFile << "Used BVH: " << useBVH << "\n" << std::endl;
}
