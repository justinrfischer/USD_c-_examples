#include "01_load_mesh.h"
#include "utils.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/gf/vec3f.h"
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

void runLoadMesh(const std::string& assetPath) {
    std::cout << "\n=== Example 01: Load Mesh ===\n";

    auto stage = UsdStage::Open(assetPath);
    if (!stage) {
        std::cerr << "Failed to open: " << assetPath << "\n";
        return;
    }
    std::cout << "Opened stage: " << assetPath << "\n";

    // Traverse all prims to find the first UsdGeomMesh
    UsdGeomMesh mesh;
    for (const auto& prim : stage->Traverse()) {
        if (prim.IsA<UsdGeomMesh>()) {
            mesh = UsdGeomMesh(prim);
            break;
        }
    }
    if (!mesh) {
        std::cerr << "No UsdGeomMesh found in stage\n";
        return;
    }
    std::cout << "Mesh path: " << mesh.GetPath() << "\n";

    // Read vertex positions — Get() returns false if attribute missing or type mismatch
    VtArray<GfVec3f> points;
    if (!mesh.GetPointsAttr().Get(&points)) {
        std::cerr << "Failed to read points attribute\n";
        return;
    }
    std::cout << "Vertex count: " << points.size() << "\n";
    std::cout << "First 5 vertices:\n";
    printVec3fArray(points, 5);

    // Read face topology
    VtArray<int> faceVertexCounts;
    mesh.GetFaceVertexCountsAttr().Get(&faceVertexCounts);
    std::cout << "Face count: " << faceVertexCounts.size() << "\n";

    VtArray<int> faceVertexIndices;
    mesh.GetFaceVertexIndicesAttr().Get(&faceVertexIndices);
    std::cout << "Face-vertex index count: " << faceVertexIndices.size() << "\n";
    if (!faceVertexCounts.empty()) {
        std::cout << "First face indices: ";
        for (int i = 0; i < faceVertexCounts[0]; ++i) {
            std::cout << faceVertexIndices[i] << " ";
        }
        std::cout << "\n";
    }
}
