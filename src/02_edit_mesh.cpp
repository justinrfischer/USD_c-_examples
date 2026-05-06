#include "02_edit_mesh.h"
#include "utils.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/gf/vec3f.h"
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

void runEditMesh(const std::string& assetPath, const std::string& outputPath) {
    std::cout << "\n=== Example 02: Edit Mesh ===\n";

    auto stage = UsdStage::Open(assetPath);
    if (!stage) { std::cerr << "Failed to open: " << assetPath << "\n"; return; }

    UsdGeomMesh mesh;
    for (const auto& prim : stage->Traverse()) {
        if (prim.IsA<UsdGeomMesh>()) { mesh = UsdGeomMesh(prim); break; }
    }
    if (!mesh) { std::cerr << "No UsdGeomMesh found\n"; return; }

    // Read current points
    VtArray<GfVec3f> points;
    mesh.GetPointsAttr().Get(&points);
    std::cout << "Before edit, vertex[0]: "
              << points[0][0] << ", " << points[0][1] << ", " << points[0][2] << "\n";

    // Translate all vertices by (0, 1, 0) — Y-up lift
    const GfVec3f offset(0.0f, 1.0f, 0.0f);
    for (auto& p : points) {
        p += offset;
    }

    // Write back to the attribute on the stage in memory
    mesh.GetPointsAttr().Set(points);
    std::cout << "After edit,  vertex[0]: "
              << points[0][0] << ", " << points[0][1] << ", " << points[0][2] << "\n";
    std::cout << "Translated " << points.size() << " vertices by (0, 1, 0)\n";

    // Export to a new file — leaves the source file unchanged
    stage->Export(outputPath);
    std::cout << "Written to: " << outputPath << "\n";
}
