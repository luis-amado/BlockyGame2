cmake_minimum_required(VERSION 3.10)
project(Luiscraft VERSION 1.0 LANGUAGES C CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ALL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/*.c"
)

add_executable(main ${ALL_SOURCES})

# Includes
include_directories(
  include
  include/imgui
  src/engine
)

target_compile_options(main PRIVATE $<$<CONFIG:Debug>:-g -D DEBUG -Wall -Wno-deprecated>)

target_compile_options(main PRIVATE $<$<CONFIG:Development>:-O3 -D DEBUG -Wno-deprecated>)

target_compile_options(main PRIVATE $<$<CONFIG:Release>:-O3>)

# Link libraries
target_link_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(main 
    PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw.3.dylib"
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)

# Set RPATH
set_target_properties(main PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION "${CMAKE_BINARY_DIR}")