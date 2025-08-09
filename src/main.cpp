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
    auto expected = mmh::MMinHook<void>::Create(
        OriginalFunction,
        DetourFunction
    );
    if (!expected) {
        std::println(
            std::cerr,
            "Failed to create MMinHook with error: {}",
            static_cast<int>(expected.error())
        );
    }
    mmh::MMinHook hook = std::move(expected.value());
    OriginalFunction();
    if (const mmh::Status status = hook.Enable(true);
        status != mmh::Status::Ok) {
        std::println(
            std::cerr,
            "Failed to enable hook with error: {}",
            static_cast<int>(status)
        );
    }
    OriginalFunction();
    hook.CallOriginal();
    return 0;
}
