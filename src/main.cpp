import mmh;
import std;

namespace {
__declspec(noinline) void OriginalFunction() {
    std::cout << "OriginalFunction" << std::endl;
}

__declspec(noinline) void DetourFunction() {
    std::cout << "DetourFunction" << std::endl;
}
} // namespace

int main() {
    mmh::MMinHook<void> hook {
        OriginalFunction,
        DetourFunction
    };
    OriginalFunction();
    hook.Enable(true);
    OriginalFunction();
    hook.CallOriginal();
    return 0;
}
