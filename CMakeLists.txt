cmake_minimum_required(VERSION 3.10)
project(Luiscraft VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. OpenGL & Platform Discovery ---
if(APPLE)
    find_library(OPENGL_FW OpenGL)
    if(OPENGL_FW)
        if(NOT TARGET OpenGL::GL)
            add_library(OpenGL::GL INTERFACE IMPORTED)
            target_link_libraries(OpenGL::GL INTERFACE "-framework OpenGL")
        endif()
    endif()
    set(PLATFORM_LIBS "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
    set(GLFW_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw.3.dylib")
elseif(WIN32)
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS "gdi32" "shell32" "user32")
    set(GLFW_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib") 
endif()

# --- 2. Sources ---
file(GLOB_RECURSE ALL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/*.c"
)
list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# --- 3. Targets ---
add_library(LuiscraftLib STATIC ${ALL_SOURCES})

# Use PUBLIC so any executable linking to this library gets the same includes
target_include_directories(LuiscraftLib PUBLIC 
    include 
    include/imgui
    src 
    src/engine
)

# --- 4. The Macro Fix ---
# We use PUBLIC so main.cpp and tests can see the path
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/" RES_PATH)
if(NOT RES_PATH MATCHES "/$")
    set(RES_PATH "${RES_PATH}/")
endif()

target_compile_definitions(LuiscraftLib PUBLIC RESOURCES_PATH="${RES_PATH}")

# --- 5. Compiler Options ---
if(MSVC)
    target_compile_options(LuiscraftLib PRIVATE /W4 /Z7)
    target_compile_definitions(LuiscraftLib PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(LuiscraftLib PRIVATE $<$<CONFIG:Debug>:-g -Wall -Wno-deprecated>)
    target_compile_options(LuiscraftLib PRIVATE $<$<CONFIG:Development>:-O3 -D DEBUG -Wno-deprecated>)
    target_compile_options(LuiscraftLib PRIVATE $<$<CONFIG:Release>:-O3>)
endif()

# --- 6. Executables ---
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE LuiscraftLib OpenGL::GL ${GLFW_LIB} ${PLATFORM_LIBS})

# RPATH for macOS
set_target_properties(main PROPERTIES 
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

# --- 7. Testing ---
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE ALL_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
if(ALL_TEST_SOURCES)
    add_executable(unit_tests ${ALL_TEST_SOURCES})
    target_link_libraries(unit_tests PRIVATE LuiscraftLib gtest_main OpenGL::GL ${GLFW_LIB} ${PLATFORM_LIBS})
    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()