find_package(Vulkan QUIET)
if (Vulkan_FOUND)
    set(VulkanEnable 1 PARENT_SCOPE)

    add_library(Vulkan SHARED
        main.cpp)
    target_include_directories(Vulkan PRIVATE ${Vulkan_INCLUDE_DIR})
    target_link_libraries(Vulkan PRIVATE ${Vulkan_LIBRARIES})
else ()
    set(VulkanEnable 0 PARENT_SCOPE)
endif ()