find_package(OpenGL QUIET)
if (OpenGL_FOUND)
    set(OpenGLEnable 1 PARENT_SCOPE)

    add_library(OpenGL SHARED
        main.cpp)
    target_include_directories(OpenGL PRIVATE ${OPENGL_INCLUDE_DIR})
    target_link_libraries(OpenGL PRIVATE ${OPENGL_LIBRARIES})
else ()
    set(OpenGLEnable 0 PARENT_SCOPE)
endif ()