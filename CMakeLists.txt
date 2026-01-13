cmake_minimum_required ( VERSION 3.1.0...4.0.0 )
project ( RayTracer CXX )

# Set to C++11
set ( CMAKE_CXX_STANDARD          11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

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

add_executable ( RayTracer ${EXTERNAL} ${SOURCE} )