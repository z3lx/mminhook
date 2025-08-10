import mmh;
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
    mmh::MMinHook<void> hook {};
    if (auto result = mmh::MMinHook<void>::Create(
            OriginalFunction,
            DetourFunction
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
    return 0;
}
