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
list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

enable_testing()

add_library(LuiscraftLib STATIC ${ALL_SOURCES})
target_include_directories(LuiscraftLib PUBLIC include src src/engine)
target_compile_definitions(LuiscraftLib PRIVATE RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE LuiscraftLib)

# Includes
include_directories(
  include
  include/imgui
  src/engine
  src
)

target_compile_options(LuiscraftLib PRIVATE $<$<CONFIG:Debug>:-g -D DEBUG -Wall -Wno-deprecated>)

target_compile_options(LuiscraftLib PRIVATE $<$<CONFIG:Development>:-O3 -D DEBUG -Wno-deprecated>)

target_compile_options(LuiscraftLib PRIVATE $<$<CONFIG:Release>:-O3>)

# Link libraries
target_link_directories(LuiscraftLib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(main 
    PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw.3.dylib"
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)

# Set RPATH
set_target_properties(LuiscraftLib PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE ALL_TEST_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)
add_executable(unit_tests ${ALL_TEST_SOURCES})
target_link_libraries(unit_tests 
  PRIVATE 
  LuiscraftLib 
  gtest_main 
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw.3.dylib"
  "-framework OpenGL"
  "-framework Cocoa"
  "-framework IOKit"
  "-framework CoreVideo"
)

include(GoogleTest)
gtest_discover_tests(unit_tests)