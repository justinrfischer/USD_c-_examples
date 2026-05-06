cmake_minimum_required(VERSION 3.20)
project(usd-mesh-lab CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point CMake at the local USD install
set(pxr_DIR $ENV{HOME}/usd-install)
find_package(pxr REQUIRED)

add_executable(usd-mesh-lab
    src/main.cpp
    src/01_load_mesh.cpp
    src/02_edit_mesh.cpp
    src/03_normals_uvs.cpp
    src/04_blend_shapes.cpp
    src/05_skinning.cpp
)

target_include_directories(usd-mesh-lab PRIVATE
    $ENV{HOME}/usd-install/include
    src/
    /Library/Frameworks/Python.framework/Versions/3.13/include/python3.13
)

target_link_libraries(usd-mesh-lab PRIVATE
    usd
    usdGeom
    usdSkel
    gf
    vt
    tf
    sdf
)

# Embed rpath so macOS finds the dylibs without DYLD_LIBRARY_PATH
set_target_properties(usd-mesh-lab PROPERTIES
    BUILD_RPATH "$ENV{HOME}/usd-install/lib"
)
