cmake_minimum_required(VERSION 3.31)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(galactic)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

#[[FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)]]
#[[
FetchContent_Declare(
        tinyobjloader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG release
)
]]

#[[FetchContent_MakeAvailable(stb tinyobjloader)]]

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED sdl3)

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

        #[[stb_image.cpp]])

target_link_libraries(galactic
        PRIVATE
        Vulkan::Vulkan
        ${SDL3_LIBRARIES}
        glm::glm
        #[[tinyobjloader]]
)
target_include_directories(galactic PRIVATE
        ${SDL3_INCLUDE_DIRS}
#[[
        ${stb_SOURCE_DIR}
]]
)

target_compile_definitions(galactic PRIVATE
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        VULKAN_HPP_NO_STRUCT_SETTERS
        GLM_FORCE_DEPTH_ZERO_TO_ONE
)