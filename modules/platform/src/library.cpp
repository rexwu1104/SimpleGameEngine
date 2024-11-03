#include <iostream>
#include <windows.h>
#include <platform/library.h>

#if defined(GLE)
#define hasOpenGL 1
#else
#define hasOpenGL 0
#endif
#if defined(VKE)
#define hasVulkan 1
#else
#define hasVulkan 0
#endif
#if defined(DXE)
#define hasDirectX 1
#else
#define hasDirectX 0
#endif
#if defined(MLE)
#define hasMetal 1
#else
#define hasMetal 0
#endif

LibraryLoader::LibraryLoader(const Library library) {
    if (library == OpenGL && hasOpenGL) {
        library_path = "./libOpenGL.dll";
    } else if (library == Vulkan && hasVulkan) {
        library_path = "./libVulkan.dll";
    } else if (library == DirectX && hasDirectX) {
        library_path = "./libDirectX.dll";
    } else if (library == Metal && hasMetal) {
        library_path = "./libMetal.dll";
    }

    if (library_path.empty()) {
        std::cerr << "Library does not exist" << std::endl;
        return;
    }
}

#if defined(WIN32)
void *LibraryLoader::get_func(const char* func_name) {
    return (void*)GetProcAddress(static_cast<HMODULE>(library_handle), func_name);
}

void LibraryLoader::load() {
    library_handle = LoadLibrary(library_path.c_str());
    if (!library_handle) {
        std::cerr << "Failed to load library" << std::endl;
        return;
    }

    call("init");
}
#endif