cmake_minimum_required(VERSION 3.12)

set(VCPKG_TARGET_ARCHITECTURE x64)

project(CompNoel)

file(GLOB SOURCES
    "*.cpp"
    "*.h"
)

add_executable(CompNoel ${SOURCES})

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(CompNoel PRIVATE glfw glm GLEW::GLEW)

target_compile_definitions(CompNoel PUBLIC GLM_FORCE_SWIZZLE)

set_target_properties(CompNoel PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED true
)
