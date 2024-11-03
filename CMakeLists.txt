cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/arc)

add_subdirectory(modules/platform)
add_subdirectory(modules/utils)
add_subdirectory(modules/directx)
add_subdirectory(modules/metal)
add_subdirectory(modules/opengl)
add_subdirectory(modules/vulkan)

include_directories(
    include
    include/utils)
add_executable(GameEngine main.cpp)

target_link_libraries(GameEngine PUBLIC Platform)
target_link_libraries(GameEngine PUBLIC Utils)
if (DirectXEnable)
    message("DirectX enabled.")
    target_compile_definitions(Platform PRIVATE DXE)
endif ()
if (OpenGLEnable)
    message("OpenGL enabled.")
    target_compile_definitions(Platform PRIVATE GLE)
endif ()
if (VulkanEnable)
    message("Vulkan enabled.")
    target_compile_definitions(Platform PRIVATE VKE)
endif ()