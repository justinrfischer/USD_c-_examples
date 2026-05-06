# Mesh Math — Interview Reference

## Linear Blend Skinning (LBS)

For each output vertex position:

```
P_world = Σ_i ( w_i * M_joint_i * M_bind_inv_i * P_local )
```

Where:
- `P_local` — vertex position in bind pose (mesh local space)
- `M_bind_inv_i` — inverse of joint i's world matrix at bind time
- `M_joint_i` — joint i's current world matrix (animated)
- `w_i` — blend weight for joint i (all weights sum to 1.0)

**In USD:** `M_bind_inv` comes from `Skeleton.bindTransforms` (stored as the forward matrix; invert in code). `M_joint` comes from `SkelAnimation`. The combined `M_joint * M_bind_inv` is called the **skinning matrix**.

**elementSize:** In `primvars:skel:jointIndices`, `elementSize = N` means each vertex has N influences. The array is flat: vertex 0 uses indices[0..N-1], vertex 1 uses indices[N..2N-1], etc.

**LBS limitation:** Collapsing volume at joints (the "candy wrapper twist" artifact). Fixed by Dual Quaternion Skinning (DQS), which blends rotations in quaternion space instead.

## Blend Shape Application

```
P_final = P_base + Σ_i ( weight_i * delta_i )
```

Where `delta_i` is the per-vertex offset for blend shape i.

**Sparse blend shapes:** `pointIndices` is present — only listed vertices have non-zero offsets. More common for facial blend shapes where only part of the mesh moves.

**Dense blend shapes:** `pointIndices` is absent — `offsets` has one entry per mesh vertex.

**Combination order matters:** If blend shapes are correlated (e.g., two jaw shapes), apply them in the order the DCC tool authored them. USD stores targets as an ordered array.

## Face Normal Recomputation

For a polygon face with vertices v0, v1, v2 (in order):

```
e1 = v1 - v0
e2 = v2 - v0
N_face = normalize(cross(e1, e2))
```

**Winding order** determines which way the normal points. USD convention: counter-clockwise winding = outward-facing normal when viewed from outside.

## Smooth Vertex Normals

1. For each face, compute the face normal.
2. Accumulate the face normal onto each vertex that face touches.
3. Normalize each accumulated vertex normal.

```cpp
VtArray<GfVec3f> smooth(numVerts, GfVec3f(0,0,0));
// ... accumulate face normals ...
for (auto& n : smooth) n.Normalize();
```

**Area-weighting:** The code above accumulates the *unnormalized* cross product, whose magnitude is proportional to face area. This means larger faces contribute more to the vertex normal — this IS area-weighted accumulation, which gives better results on irregular meshes. For equal-weight accumulation, normalize the face normal before accumulating.

## UV Interpolation Modes (again, because it comes up)

- **faceVarying** UV: 24 values for a 6-face cube (4 per face). Each face corner has an independent UV. This allows seams — UV island boundaries.
- **vertex** UV: 8 values for an 8-vertex cube. No seams possible. Only valid for seamless spherical mapping or similar.

**Why faceVarying is almost always correct for characters:** Avatars have UV seams (neck, armpits, inside ears). Any atlas-based texture requires faceVarying UVs to express those seams.

## Normal Map Spaces

- **Tangent space:** Normal map encodes normals relative to the surface. Portable across deformations and transforms. Most common for characters.
- **Object space:** Normals relative to the mesh's local coordinate system. Cheaper to sample but breaks with skinning unless rebaked.
- **World space:** Rare, used for static architecture bakes.

For avatar work: always assume tangent space unless otherwise specified.
