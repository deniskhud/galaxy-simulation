cmake_minimum_required(VERSION 3.31)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(galactic)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(imgui PUBLIC
        Vulkan::Vulkan
        ${SDL3_LIBRARIES}
)

add_executable(galactic main.cpp
        src/window/window.hpp
        src/window/window.cpp
        src/core/context.hpp
        src/core/context.cpp
        src/core/context.hpp
        src/core/swapchain.hpp
        src/core/swapchain.cpp
        src/pipelines/pipeline.hpp
        src/pipelines/pipeline.cpp
        src/camera/camera.hpp
        src/scene/particle.hpp
        src/core/buffer.hpp
        src/core/buffer.cpp
        src/core/descriptors.hpp
        src/core/descriptors.cpp
        src/scene/particle.cpp
        src/camera/camera.cpp
        src/renderer/renderer.hpp
        src/renderer/renderer.hpp
        src/renderer/renderer.hpp
        src/renderer/renderer.cpp
        src/include/inc.hpp
        src/debugSystem.hpp
        src/engine/engine.hpp
        src/engine/engine.cpp

)

target_link_libraries(galactic
        PRIVATE
        Vulkan::Vulkan
        ${SDL3_LIBRARIES}
        glm::glm
        imgui
)
target_include_directories(galactic PRIVATE
        ${SDL3_INCLUDE_DIRS}
)

target_compile_definitions(galactic PRIVATE
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_NO_STRUCT_SETTERS
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)