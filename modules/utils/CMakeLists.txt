add_library(Utils STATIC
    main.cpp
    src/logger/logger.cpp
    src/logger/channel.cpp
    src/logger/format.cpp)

target_include_directories(Utils PRIVATE ${CMAKE_SOURCE_DIR}/include/utils)