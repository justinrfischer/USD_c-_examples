#include "06_smooth.h"
#include "utils.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/tf/token.h"
#include <iostream>
#include <algorithm>

PXR_NAMESPACE_USING_DIRECTIVE

void runSmooth(const std::string& assetPath, const std::string& outputPath, int iterations = 10) {

    // get the stage
    UsdStageRefPtr stage = UsdStage::Open(assetPath);
    if(!stage){
        std::cerr << "failed to open stage!: " << assetPath << "\n";
        return;
    }
    // find the mesh
    UsdGeomMesh mesh; 
    for(const auto& prim : stage->Traverse()){
        if(prim.IsA<UsdGeomMesh>()){
            mesh = UsdGeomMesh(prim);
            break;
        }
    }

    // read mesh data (points, faceVertexCounts, faceVertexIndices)
    VtArray<GfVec3f> points;
    mesh.GetPointsAttr().Get(&points);
    VtArray<int> faceVertexCounts, faceVertexIndices;
    mesh.GetFaceVertexCountsAttr().Get(&faceVertexCounts);
    mesh.GetFaceVertexIndicesAttr().Get(&faceVertexIndices);

    // compute smooth positions by accumulating vertex neighbor face positions and normalizing
    VtArray<GfVec3f> smoothPoints(points.size(), GfVec3f(0.0f, 0.0f, 0.0f));
    VtArray<int> vertexValence(points.size(), 0);

    // add and iterations loop to smooth many times for better results
    for (int iter = 0; iter < iterations; ++iter) {
        std::cout << "Smoothing iteration: " << iter + 1 << "\n";
        // reset smoothPoints and vertexValence for this iteration
        std::fill(smoothPoints.begin(), smoothPoints.end(), GfVec3f(0.0f, 0.0f, 0.0f));
        std::fill(vertexValence.begin(), vertexValence.end(), 0);
        int idx = 0;    
        for(int faceVerts : faceVertexCounts){
            // compute face center
            GfVec3f faceCenter(0.0f, 0.0f, 0.0f);
            for(int i = 0; i < faceVerts; ++i){
                int vertIdx = faceVertexIndices[idx + i];
                faceCenter += points[vertIdx];
            }
            faceCenter /= faceVerts;            
            // accumulate face center onto each vertex of the face
            for(int i = 0; i < faceVerts; ++i){
                int vertIdx = faceVertexIndices[idx + i];
                smoothPoints[vertIdx] += faceCenter;
                vertexValence[vertIdx] += 1;
            }
            idx += faceVerts;
        }

        // set the smooth point for each vertex by normalizing the accumulated face centers by the vertex valence
        for(int i = 0; i < (int)smoothPoints.size(); ++i){
            if(vertexValence[i] > 0){
                smoothPoints[i] /= vertexValence[i];
            }
        }
        // update points for the next iteration
        points = smoothPoints;
        mesh.GetPointsAttr().Set(smoothPoints);
    }   

    // write the new positions back into the mesh and save the stage to outputPath
    stage->Export(outputPath);
    std::cout << "Saved smoothed mesh to: " << outputPath << "\n";
}