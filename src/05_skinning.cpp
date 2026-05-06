#include "05_skinning.h"
#include "utils.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdSkel/root.h"
#include "pxr/usd/usdSkel/skeleton.h"
#include "pxr/usd/usdSkel/bindingAPI.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/tf/token.h"
#include <iostream>
#include <algorithm>

PXR_NAMESPACE_USING_DIRECTIVE

void runSkinning(const std::string& assetPath) {
    std::cout << "\n=== Example 05: Skinning ===\n";

    auto stage = UsdStage::Open(assetPath);
    if (!stage) { std::cerr << "Failed to open: " << assetPath << "\n"; return; }

    // Find SkelRoot
    UsdSkelRoot skelRoot;
    for (const auto& prim : stage->Traverse()) {
        if (prim.IsA<UsdSkelRoot>()) { skelRoot = UsdSkelRoot(prim); break; }
    }
    if (!skelRoot) { std::cerr << "No UsdSkelRoot found\n"; return; }
    std::cout << "SkelRoot: " << skelRoot.GetPath() << "\n";

    // Find Skeleton child
    UsdSkelSkeleton skeleton;
    for (const auto& child : skelRoot.GetPrim().GetChildren()) {
        if (child.IsA<UsdSkelSkeleton>()) { skeleton = UsdSkelSkeleton(child); break; }
    }
    if (!skeleton) { std::cerr << "No Skeleton found\n"; return; }

    VtArray<TfToken> joints;
    skeleton.GetJointsAttr().Get(&joints);
    std::cout << "Joints (" << joints.size() << "):\n";
    for (const auto& j : joints) {
        std::cout << "  " << j << "\n";
    }

    // Find mesh with skinning primvars
    for (const auto& prim : stage->Traverse()) {
        if (!prim.IsA<UsdGeomMesh>()) continue;

        UsdSkelBindingAPI binding(prim);
        UsdGeomPrimvar weightsPV  = binding.GetJointWeightsPrimvar();
        UsdGeomPrimvar indicesPV  = binding.GetJointIndicesPrimvar();
        if (!weightsPV || !indicesPV) continue;

        VtArray<float> weights;
        VtArray<int>   indices;
        weightsPV.Get(&weights);
        indicesPV.Get(&indices);
        int elementSize = weightsPV.GetElementSize();

        std::cout << "\nMesh: " << prim.GetPath() << "\n";
        std::cout << "Influences per vertex (elementSize): " << elementSize << "\n";

        int numVerts = (int)weights.size() / elementSize;
        int showVerts = std::min(numVerts, 5);
        std::cout << "First " << showVerts << " vertices:\n";
        for (int v = 0; v < showVerts; ++v) {
            std::cout << "  Vertex[" << v << "]: ";
            for (int inf = 0; inf < elementSize; ++inf) {
                int   jIdx = indices[v * elementSize + inf];
                float w    = weights[v * elementSize + inf];
                if (w > 0.0f && jIdx < (int)joints.size()) {
                    std::cout << joints[jIdx] << "=" << w << " ";
                }
            }
            std::cout << "\n";
        }

        // Read bind transform
        UsdAttribute geomBindAttr = binding.GetGeomBindTransformAttr();
        if (geomBindAttr) {
            GfMatrix4d bindXform;
            geomBindAttr.Get(&bindXform);
            std::cout << "GeomBindTransform translation: ("
                      << bindXform[3][0] << ", "
                      << bindXform[3][1] << ", "
                      << bindXform[3][2] << ")\n";
        }
        break;
    }
}
