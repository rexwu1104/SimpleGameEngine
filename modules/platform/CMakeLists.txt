set(Files
    src/library.cpp
    main.cpp)
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_library(Platform STATIC
        ${Files})
    target_include_directories(Platform PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/platform/win)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_library(Platform STATIC
        ${Files})
    target_include_directories(Platform PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/platform/linux)
else ()
    add_library(Platform STATIC
        ${Files})
    target_include_directories(Platform PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/platform/darwin)
endif ()