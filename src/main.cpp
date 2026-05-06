#include "01_load_mesh.h"
#include "02_edit_mesh.h"
#include "03_normals_uvs.h"
#include "04_blend_shapes.h"
#include "05_skinning.h"
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    const std::string assetPath = "../assets/avatar.usda";

    auto printMenu = []() {
        std::cout << "\n=== USD Mesh Lab ===\n"
                  << "1  Load Mesh      - points, face topology, primvars\n"
                  << "2  Edit Mesh      - translate vertices, export\n"
                  << "3  Normals & UVs  - recompute normals, read faceVarying UVs\n"
                  << "4  Blend Shapes   - sparse offsets, write new target\n"
                  << "5  Skinning       - joint weights and bind transform\n"
                  << "0  Run all\n"
                  << "Choice: ";
    };

    int choice = 0;
    if (argc > 1) {
        choice = std::atoi(argv[1]);
    } else {
        printMenu();
        std::cin >> choice;
    }

    switch (choice) {
        case 0:
            runLoadMesh(assetPath);
            runEditMesh(assetPath, "output_02_edited.usda");
            runNormalsUVs(assetPath, "output_03_normals.usda");
            runBlendShapes(assetPath, "output_04_blendshapes.usda");
            runSkinning(assetPath);
            break;
        case 1: runLoadMesh(assetPath); break;
        case 2: runEditMesh(assetPath, "output_02_edited.usda"); break;
        case 3: runNormalsUVs(assetPath, "output_03_normals.usda"); break;
        case 4: runBlendShapes(assetPath, "output_04_blendshapes.usda"); break;
        case 5: runSkinning(assetPath); break;
        default:
            std::cerr << "Invalid choice. Pass 0-5 as argument.\n";
            return 1;
    }
    return 0;
}
