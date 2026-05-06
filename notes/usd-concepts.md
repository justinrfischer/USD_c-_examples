# USD Concepts

## The Core Object Model

**Stage** — the composed scene. A view over one or more layers. Think of it as the final, resolved scene you see.

**Layer** — a single USD file on disk (`.usda`, `.usdc`, `.usdz`). Stores opinion data. Multiple layers compose into a Stage. The **root layer** is the layer you open; it can include others via `subLayers`.

**Prim** — a node in the scene graph. Has a path like `/Avatar/Body`. Types are registered schemas: `UsdGeomMesh`, `UsdSkelSkeleton`, etc. `prim.IsA<T>()` checks type.

**Property** — data hanging off a prim. Two kinds:
- **Attribute** — typed value (e.g. `point3f[] points`). Can be time-sampled or default.
- **Relationship** — typed pointer to another prim or property (e.g. `rel skel:skeleton = </Avatar/Skeleton>`).

**Primvar** — an attribute marked for rasterization interpolation. Lives in the `primvars:` namespace. Has an **interpolation** mode that controls how values map onto geometry.

## Primvar Interpolation Modes

| Mode | Values per prim | Typical use |
|------|----------------|-------------|
| `constant` | 1 | Single value for whole mesh |
| `uniform` | 1 per face | Face color |
| `vertex` | 1 per vertex | Smooth normals, joint weights |
| `faceVarying` | 1 per face-vertex | UVs (each face corner has its own) |

**Note:** UVs are almost always `faceVarying` because a cube's corner needs different UV coordinates for each of the 3 faces it touches. `vertex` interpolation cannot express seams.

## Composition Arcs — LIVRPS (strength order, strongest first)

- **L** — Local opinions (opinions from any layer in the active layer stack, evaluated strongest-to-weakest — not just the "current" layer; a sublayer override is still a Local opinion and beats Inherits)
- **I** — Inherits (class-based inheritance via `inherits` arc)
- **V** — VariantSets (switchable variations, e.g. LOD or material)
- **R** — References (include another file/prim)
- **P** — Payload (lazy-loaded reference for large assets)
- **S** — SubLayers (layer stack composition)

Stronger opinions win. A local `points` attribute overrides the same attribute from a referenced file.

## VtArray

`VtArray<T>` is USD's typed array. It is **copy-on-write** — reads are cheap, a write triggers a copy. The canonical pattern:

```cpp
VtArray<GfVec3f> points;
attr.Get(&points);           // read
for (auto& p : points) { p += offset; }  // triggers COW copy
attr.Set(points);            // write back
```

## UsdTimeCode

Attributes can be time-sampled. `Get(&value)` with no TimeCode reads the **default** (non-animated) value. For animated data:

```cpp
attr.Get(&points, UsdTimeCode(42.0));   // frame 42
```

Static meshes use `UsdTimeCode::Default()`. Always pass a TimeCode when reading animated rigs.

## UsdSkel Two-Level Hierarchy

```
SkelRoot             ← required ancestor; controls bounding volume
  Skeleton           ← joint hierarchy (names as token paths: "Root/Spine/Chest")
  SkelAnimation      ← time-sampled joint transforms
  Mesh (SkelBindingAPI applied)
    rel skel:skeleton → Skeleton
    primvars:skel:jointIndices  (elementSize = max influences)
    primvars:skel:jointWeights  (same elementSize)
    skel:geomBindTransform      ← mesh's bind-pose world matrix
```

`skel:geomBindTransform` exists because a mesh may have been authored in a different coordinate space than the skeleton. It converts mesh vertices into skeleton space before skinning.

## Save vs Export

- `stage->Save()` — writes to the **root layer's** original file path. Mutates in place.
- `stage->Export(path)` — writes a **flattened** single-layer copy to a new path. Safe for producing deliverables without touching source files.

Always use `Export` so you never accidentally corrupt the source asset.
