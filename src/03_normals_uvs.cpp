#include "03_normals_uvs.h"
#include "utils.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/primvarsAPI.h"
#include "pxr/usd/usdGeom/tokens.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/tf/token.h"
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

void runNormalsUVs(const std::string& assetPath, const std::string& outputPath) {
    std::cout << "\n=== Example 03: Normals & UVs ===\n";

    auto stage = UsdStage::Open(assetPath);
    if (!stage) { std::cerr << "Failed to open: " << assetPath << "\n"; return; }

    UsdGeomMesh mesh;
    for (const auto& prim : stage->Traverse()) {
        if (prim.IsA<UsdGeomMesh>()) { mesh = UsdGeomMesh(prim); break; }
    }
    if (!mesh) { std::cerr << "No UsdGeomMesh found\n"; return; }

    // --- Read UV primvar ---
    UsdGeomPrimvarsAPI pvAPI(mesh.GetPrim());
    auto stPrimvar = pvAPI.GetPrimvar(TfToken("st"));
    if (stPrimvar) {
        VtArray<GfVec2f> uvs;
        stPrimvar.Get(&uvs);
        TfToken interp = stPrimvar.GetInterpolation();
        std::cout << "UV primvar 'st': " << uvs.size() << " values, interpolation=" << interp << "\n";
        std::cout << "  First UV: " << uvs[0][0] << ", " << uvs[0][1] << "\n";
        std::cout << "  Last UV:  " << uvs.back()[0] << ", " << uvs.back()[1] << "\n";
    } else {
        std::cout << "No 'st' primvar found\n";
    }

    // --- Read existing normals ---
    VtArray<GfVec3f> existingNormals;
    mesh.GetNormalsAttr().Get(&existingNormals);
    std::cout << "Existing normals count: " << existingNormals.size()
              << ", interpolation: " << mesh.GetNormalsInterpolation() << "\n";

    // --- Recompute smooth vertex normals ---
    VtArray<GfVec3f> points;
    mesh.GetPointsAttr().Get(&points);
    VtArray<int> faceVertexCounts, faceVertexIndices;
    mesh.GetFaceVertexCountsAttr().Get(&faceVertexCounts);
    mesh.GetFaceVertexIndicesAttr().Get(&faceVertexIndices);

    // Accumulate face normals onto each vertex
    VtArray<GfVec3f> smoothNormals(points.size(), GfVec3f(0.0f, 0.0f, 0.0f));
    int idx = 0;
    for (int faceVerts : faceVertexCounts) {
        int i0 = faceVertexIndices[idx];
        int i1 = faceVertexIndices[idx + 1];
        int i2 = faceVertexIndices[idx + 2];
        GfVec3f e1 = points[i1] - points[i0];
        GfVec3f e2 = points[i2] - points[i0];
        GfVec3f faceNormal = GfCross(e1, e2);
        for (int j = 0; j < faceVerts; ++j) {
            smoothNormals[faceVertexIndices[idx + j]] += faceNormal;
        }
        idx += faceVerts;
    }
    for (auto& n : smoothNormals) {
        n.Normalize();
    }

    // Write back
    mesh.GetNormalsAttr().Set(smoothNormals);
    mesh.SetNormalsInterpolation(UsdGeomTokens->vertex);
    std::cout << "Recomputed " << smoothNormals.size() << " smooth vertex normals\n";
    std::cout << "  Normal[0]: "
              << smoothNormals[0][0] << ", "
              << smoothNormals[0][1] << ", "
              << smoothNormals[0][2] << "\n";

    stage->Export(outputPath);
    std::cout << "Written to: " << outputPath << "\n";
}
