import mmh;
// #include "mmh/Hook.hpp"
// #include "mmh/ex/Hook.hpp"
import std;

namespace {
__declspec(noinline) void OriginalFunction() {
    std::println("OriginalFunction");
}

__declspec(noinline) void DetourFunction() {
    std::println("DetourFunction");
}
} // namespace

int main() {
    {
        mmh::Hook<void> hook {};
        if (auto result = mmh::Hook<void>::Create(
                OriginalFunction,
                DetourFunction,
                true
            );
            !result) {
            std::println(
                std::cerr,
                "Failed to create hook with error: {}",
                static_cast<int>(result.error())
            );
            return 1;
        } else {
            hook = std::move(result.value());
        }

        OriginalFunction();
        if (const auto result = hook.Enable(true);
            !result) {
            std::println(
                std::cerr,
                "Failed to enable hook with error: {}",
                static_cast<int>(result.error())
            );
            return 1;
        }
        OriginalFunction();
        hook.CallOriginal();
    }

    try {
        mmh::ex::Hook<void> hookA = mmh::ex::Hook<void>::Create(
            OriginalFunction,
            DetourFunction
        );
        auto hook = std::move(hookA);

        OriginalFunction();
        hook.Enable(true);
        OriginalFunction();
        hook.CallOriginal();

        hookA.CallOriginal();
    } catch (const mmh::Exception& e) {
        std::println(std::cerr, "{}", e.what());
        return 1;
    }
    return 0;
}
