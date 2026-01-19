cmake_minimum_required ( VERSION 3.10.0...4.0.0 )
project ( RayTracer CXX )

# Set to C++17
set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source

# set ( EXTERNAL
#   src/external/stb_image.h
#   src/external/stb_image_write.h
# )

file ( GLOB SOURCE
  src/main.cpp
  src/*.h
  src/render.cl
)

include_directories ( src )

# SFML

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# OpenCL
set (TCLAP_INCLUDE_PATH '-DTCLAP_INCLUDE_PATH = src/external/tclap-1.4.0-rc2/include' )
set (Stb_INCLUDE_PATH '-D Stb_INCLUDE_PATH = src/external/stb' )
set (Vulkan_INCLUDE_DIR $ENV{VULKAN_SDK}/Include)
set (Vulkan_LIBRARY $ENV{VULKAN_SDK}/Lib)
# include_directories ( TCLAP_INCLUDE_PATH Stb_INCLUDE_PATH)
include( FetchContent )
FetchContent_Declare( OpenCL
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-SDK.git
    GIT_TAG v2025.07.23
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM )
FetchContent_MakeAvailable( OpenCL )

add_executable ( RayTracer ${SOURCE} )
target_compile_features( RayTracer PRIVATE cxx_std_17 )
target_link_libraries ( RayTracer PRIVATE 
SFML::Graphics SFML::Window OpenCL::OpenCL OpenCL::HeadersCpp)