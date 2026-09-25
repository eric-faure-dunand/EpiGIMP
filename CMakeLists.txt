cmake_minimum_required(VERSION 3.16)
project(EpiGIMP CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include(FetchContent)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

add_library(glad STATIC vendor/glad/src/gl.c)
target_include_directories(glad PUBLIC vendor/glad/include)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

file(GLOB_RECURSE PROJECT_SRCS ${CMAKE_SOURCE_DIR}/project/src/*.cpp)

add_executable(EpiGIMP ${PROJECT_SRCS})

target_include_directories(EpiGIMP
    PRIVATE
        project/include
        project/include/Error
        project/include/Core
        project/include/Printer
        project/include/Canvas
        project/include/UI
)

target_include_directories(EpiGIMP SYSTEM PRIVATE vendor/stb)

target_compile_options(EpiGIMP
    PRIVATE
        -Wall
        -Wextra
        -Werror
        -std=c++20
)

target_link_libraries(EpiGIMP
    PRIVATE
        glfw
        glad
        imgui
        OpenGL::GL
)
