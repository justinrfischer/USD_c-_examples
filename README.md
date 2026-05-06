# USD Mesh Lab

C++ examples for USD mesh I/O, UsdSkel, blend shapes, normals, and skinning.

## Setup

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(sysctl -n hw.logicalcpu)
```

## Run

```bash
./usd-mesh-lab 0    # all examples
./usd-mesh-lab 1    # load mesh
./usd-mesh-lab 2    # edit mesh
./usd-mesh-lab 3    # normals & UVs
./usd-mesh-lab 4    # blend shapes
./usd-mesh-lab 5    # skinning
```

## Study Notes

- `notes/usd-concepts.md` — Stage/Layer/Prim, LIVRPS, primvar interpolation, UsdSkel
- `notes/mesh-math.md` — LBS formula, blend shapes, normal recompute, UV modes

## Key Concepts

1. **Why USD?** Composition arcs (LIVRPS) let multiple teams work on the same asset simultaneously via layers. The final scene is a non-destructive view over all contributions.
2. **Primvar interpolation** — faceVarying vs vertex is a common gotcha. UVs need faceVarying for seams.
3. **UsdSkel elementSize** — skinning data is flat; elementSize tells you how many indices/weights belong to each vertex.
4. **GeomBindTransform** — the mesh's bind-pose world matrix, needed to bring vertex positions into skeleton space before LBS.
5. **Export vs Save** — use Export for safe deliverables; Save mutates in place.
