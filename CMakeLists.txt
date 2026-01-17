cmake_minimum_required ( VERSION 3.1.0...4.0.0 )
project ( RayTracer CXX )

# Set to C++17
set ( CMAKE_CXX_STANDARD          17 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source

set ( EXTERNAL
  src/external/stb_image.h
  src/external/stb_image_write.h
)

file ( GLOB SOURCE
  src/main.cpp
  src/*.h
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

add_executable ( RayTracer ${EXTERNAL} ${SOURCE} )
target_compile_features( RayTracer PRIVATE cxx_std_17 )
target_link_libraries ( RayTracer PRIVATE SFML::Graphics SFML::Window )