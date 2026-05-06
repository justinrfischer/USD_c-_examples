#pragma once
#include "pxr/pxr.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/vt/array.h"
#include <iostream>
#include <string>

PXR_NAMESPACE_USING_DIRECTIVE

inline void printVec3fArray(const VtArray<GfVec3f>& arr, size_t maxCount = 5) {
    size_t count = std::min(arr.size(), maxCount);
    for (size_t i = 0; i < count; ++i) {
        const auto& v = arr[i];
        std::cout << "  [" << i << "] " << v[0] << ", " << v[1] << ", " << v[2] << "\n";
    }
    if (arr.size() > maxCount) {
        std::cout << "  ... (" << arr.size() << " total)\n";
    }
}
