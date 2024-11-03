#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

class LibraryLoader {
private:
    std::string library_path;
    void* library_handle = nullptr;
public:
    enum Library {
        OpenGL,
        Vulkan,
        DirectX,
        Metal
    };

    explicit LibraryLoader(Library library);
    virtual ~LibraryLoader() = default;
    virtual void load() = 0;
    virtual void* get_func(const char*) = 0;

    template<typename Ret = void, typename... Args>
    Ret call(const char* func_name, Args... args) {
        auto func = (Ret(*)(Args...))get_func(func_name);
        if constexpr (std::is_same_v<Ret, void>) {
            func(args...);
            return;
        } else {
            return func(args...);
        }
    }
};

#endif
