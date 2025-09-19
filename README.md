# mminhook
A modern C++23 interface for [MinHook](https://github.com/TsudaKageyu/minhook), with C++20 module support. Provides a lightweight, type-safe, and expressive API with flexible error handling strategies. See the [example](#example) for usage.

## Installation
MMinHook can be installed via CMake's `FetchContent` module.
- `BUILD_SHARED_LIBS` is `OFF` by default to build a static library; set to `ON` for a shared library.
- `MMH_BUILD_MODULES` is `ON` by default to enable module support; set to `OFF` to disable.
- Requires CMake 3.28.2+ and a C++23 compiler with C++20 modules support for module builds.

```cmake
include(FetchContent)

FetchContent_Declare(
    mminhook
    GIT_REPOSITORY https://github.com/z3lx/mminhook.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mminhook)

add_executable(app)
target_link_libraries(app PRIVATE mmh::mmh)
```

## Usage

Check the [header files](include/mmh) containing the public interface for exact function signatures and template parameters, as they are self-documenting, and developers already familiar with MinHook will find them intuitive.

### Consuming the library
- If using C++20 modules, import the `mmh` module with `import mmh;`.
- If not using modules, include the main header with `#include <mmh/Hook.hpp>`.

### Hooking functions
- Use the `mmh::Hook<Ret, Args...>` class template to instantiate an empty (default constructed) hook for a function with return type `Ret` and argument types `Args...`. Any method that manipulates the state of an empty hook will fail.
- Use the `mmh::Hook<Ret, Args...>::Create` static method to create a hook; its input parameters match those of `MH_CreateHook` and `MH_CreateHookApi`, with an additional boolean parameter to specify whether to enable the hook immediately after creation.
- Use the `mmh::Hook<Ret, Args...>::Enable` method to enable or disable the hook.
- Use the `mmh::Hook<Ret, Args...>::CallOriginal` method to call the trampoline to the original function.
- MinHook is initialized automatically when the first hook is created, and uninitialized when the last hook is destroyed.
- Excluding the creation of the hook, all methods are not thread-safe. Without proper synchronization, undefined behavior may occur.

### Error handling
- Methods that may fail provide both throwing and non-throwing variants; non-throwing methods use the `Try*` suffix in their names.
- Throwing methods throw `mmh::Exception` on failure; `mmh::Exception::what` returns a non-owning string describing the error, and `mmh::Exception::GetError` returns an `mmh::Error` representing the error code.
- Non-throwing methods return a `mmh::Result<T>`, which is an alias for `std::expected<T, E>`; `T` is the type of the return value on success, and `E` is of type `mmh::Error` representing the error code on failure.
- If compiling without exceptions enabled, throwing methods will instead terminate the program.

### Example
Using the library to hook the `GetAsyncKeyState` function from the Windows API:

```cpp
import mmh;
import std;

import <Windows.h>;

mmh::Hook<SHORT, int> hook {};

SHORT GetAsyncKeyStateHk(const int vKey) noexcept try {
    const SHORT state = hook.CallOriginal(vKey);
    std::println(
        std::cout,
        "GetAsyncKeyState called with vKey = 0x{0:02X}, "
        "returned with state = 0x{1:04X}",
        static_cast<unsigned int>(vKey),
        static_cast<unsigned int>(state)
    );
    return state;
} catch (...) {
    return 0;
}

int main() noexcept try {
    hook = mmh::Hook<SHORT, int>::Create(
        GetAsyncKeyState,
        GetAsyncKeyStateHk,
        true
    );
    GetAsyncKeyState(VK_RBUTTON);
    return 0;
} catch (const mmh::Exception& e) {
    std::println(
        std::cerr,
        "Caught mmh::Exception with error: {}",
        e.what()
    );
    return 1;
}
```

Possible output:
```
GetAsyncKeyState called with vKey = 0x02, returned with state = 0x0000
```

## Attribution
This project uses [`minhook`](https://github.com/TsudaKageyu/minhook), licensed under the BSD-2-Clause License.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
