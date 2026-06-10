cmake_minimum_required(VERSION 3.31)
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

        #[[stb_image.cpp]])

target_link_libraries(galactic
        PRIVATE
        Vulkan::Vulkan
      #[[  ${SDL3_LIBRARIES}]]
        glm::glm
        #[[tinyobjloader]]
)
target_include_directories(galactic PRIVATE
#[[
        ${stb_SOURCE_DIR}
]]
)