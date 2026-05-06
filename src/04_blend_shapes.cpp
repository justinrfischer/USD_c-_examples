#include "04_blend_shapes.h"
#include "utils.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdSkel/blendShape.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/vt/array.h"
#include <iostream>
#include <cmath>

PXR_NAMESPACE_USING_DIRECTIVE

void runBlendShapes(const std::string& assetPath, const std::string& outputPath) {
    std::cout << "\n=== Example 04: Blend Shapes ===\n";

    auto stage = UsdStage::Open(assetPath);
    if (!stage) { std::cerr << "Failed to open: " << assetPath << "\n"; return; }

    // Find first BlendShape prim by traversal
    UsdSkelBlendShape blendShape;
    for (const auto& prim : stage->Traverse()) {
        if (prim.IsA<UsdSkelBlendShape>()) {
            blendShape = UsdSkelBlendShape(prim);
            break;
        }
    }
    if (!blendShape) { std::cerr << "No UsdSkelBlendShape found\n"; return; }

    std::cout << "BlendShape prim: " << blendShape.GetPath() << "\n";

    // Read offsets
    VtArray<GfVec3f> offsets;
    blendShape.GetOffsetsAttr().Get(&offsets);

    // Read point indices (present = sparse, absent = dense)
    VtArray<int> pointIndices;
    blendShape.GetPointIndicesAttr().Get(&pointIndices);
    bool isSparse = !pointIndices.empty();
    std::cout << "Type: " << (isSparse ? "sparse" : "dense") << "\n";
    std::cout << "Offset count: " << offsets.size() << "\n";

    for (size_t i = 0; i < offsets.size(); ++i) {
        const auto& o = offsets[i];
        float mag = std::sqrt(o[0]*o[0] + o[1]*o[1] + o[2]*o[2]);
        int vertIdx = isSparse ? pointIndices[i] : (int)i;
        std::cout << "  Point[" << vertIdx << "] offset=("
                  << o[0] << "," << o[1] << "," << o[2]
                  << ") magnitude=" << mag << "\n";
    }

    // Write a new blend shape: ChinPuff — push bottom verts down
    auto newBS = UsdSkelBlendShape::Define(stage, SdfPath("/Avatar/ChinPuff"));
    VtArray<GfVec3f> newOffsets = { {0.0f, -0.3f, 0.0f}, {0.0f, -0.3f, 0.0f} };
    VtArray<int>     newIndices = { 0, 1 };
    newBS.CreateOffsetsAttr().Set(newOffsets);
    newBS.CreatePointIndicesAttr().Set(newIndices);
    std::cout << "Created /Avatar/ChinPuff with " << newOffsets.size() << " sparse offsets\n";

    stage->Export(outputPath);
    std::cout << "Written to: " << outputPath << "\n";
}
