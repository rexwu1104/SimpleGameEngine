if (WIN32)
    set(DirectXEnable 1 PARENT_SCOPE)
    set(WINSDK "$ENV{ProgramFiles\(x86\)}/Windows Kits/10")

    add_library(DirectX SHARED main.cpp)
    target_link_libraries(DirectX PRIVATE
        d3d12.lib
        dxgi.lib
        dxguid.lib)
else ()
    set(DirectXEnable 0 PARENT_SCOPE)
endif ()