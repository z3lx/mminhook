module;

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

export module mmh;

import std;

export namespace mmh {
enum class Error : std::int8_t {
    Unknown = MH_UNKNOWN,
    AlreadyInitialized = MH_ERROR_ALREADY_INITIALIZED,
    NotInitialized = MH_ERROR_NOT_INITIALIZED,
    AlreadyCreated = MH_ERROR_ALREADY_CREATED,
    NotCreated = MH_ERROR_NOT_CREATED,
    AlreadyEnabled = MH_ERROR_ENABLED,
    AlreadyDisabled = MH_ERROR_DISABLED,
    NotExecutable = MH_ERROR_NOT_EXECUTABLE,
    UnsupportedFunction = MH_ERROR_UNSUPPORTED_FUNCTION,
    MemoryAlloc = MH_ERROR_MEMORY_ALLOC,
    MemoryProtect = MH_ERROR_MEMORY_PROTECT,
    ModuleNotFound = MH_ERROR_MODULE_NOT_FOUND,
    FunctionNotFound = MH_ERROR_FUNCTION_NOT_FOUND
};

template <typename Ret, typename... Args>
class MMinHook {
public:
    [[nodiscard]] static std::expected<MMinHook, Error>
    Create(void* target, void* detour, bool enable = false) noexcept;

    MMinHook() noexcept;
    ~MMinHook() noexcept;

    MMinHook(const MMinHook& other) = delete;
    MMinHook& operator=(const MMinHook& other) = delete;
    MMinHook(MMinHook&& other) noexcept;
    MMinHook& operator=(MMinHook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    std::expected<void, Error> Enable(bool enable) noexcept;
    std::expected<Ret, Error> CallOriginal(Args... args) const noexcept;

private:
    MMinHook(void* target, void* original, bool isEnabled) noexcept;

    void* target;
    void* original;
    bool isEnabled;
};
} // namespace mmh

using Result = std::expected<void, mmh::Error>;
export template <auto MhFunc, typename... Args>
Result CallMinHook(Args&&... args) noexcept {
    if (const MH_STATUS status = MhFunc(std::forward<Args>(args)...);
        status != MH_OK) {
        return std::unexpected { static_cast<mmh::Error>(status) };
    }
    return {};
}

module :private;

Result InitializeMinHook(const bool initialize) noexcept {
    static std::mutex mutex {};
    static size_t referenceCount = 0;
    std::lock_guard lock { mutex };
    return initialize
        ? (referenceCount++ == 0 ? CallMinHook<MH_Initialize>() : Result {})
        : (--referenceCount == 0 ? CallMinHook<MH_Uninitialize>() : Result {});
}

namespace mmh {
template <typename Ret, typename... Args>
std::expected<MMinHook<Ret, Args...>, Error>
MMinHook<Ret, Args...>::Create(
    void* target, void* detour, const bool enable) noexcept {
    void* original = nullptr;
    const auto createHook = [=, &original]() -> Result {
        return CallMinHook<MH_CreateHook>(target, detour, &original);
    };

    const auto enableHook = [=]() -> Result {
        return enable ? CallMinHook<MH_EnableHook>(target) : Result {};
    };

    const Result result = InitializeMinHook(true)
        .and_then(createHook)
        .and_then(enableHook);
    if (!result) {
        InitializeMinHook(false);
        return std::unexpected { result.error() };
    }

    return MMinHook { target, original, enable };
}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>::MMinHook() noexcept
    : target { nullptr }, original { nullptr }, isEnabled { false } {};

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>::MMinHook(
    void* target, void* original, const bool isEnabled) noexcept
    : target { target }, original { original }, isEnabled { isEnabled } {}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>::~MMinHook() noexcept {
    if (target == nullptr) {
        return;
    }

    MH_RemoveHook(target);
    InitializeMinHook(false);
}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>::MMinHook(MMinHook&& other) noexcept
    : target { other.target }
    , original { other.original }
    , isEnabled { other.isEnabled } {
    other.target = nullptr;
    other.original = nullptr;
    other.isEnabled = false;
}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>&
MMinHook<Ret, Args...>::operator=(MMinHook&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    target = other.target;
    original = other.original;
    isEnabled = other.isEnabled;

    other.target = nullptr;
    other.original = nullptr;
    other.isEnabled = false;

    return *this;
}

template <typename Ret, typename... Args>
bool MMinHook<Ret, Args...>::IsEnabled() const noexcept {
    return isEnabled;
}

template <typename Ret, typename... Args>
std::expected<void, Error>
MMinHook<Ret, Args...>::Enable(const bool enable) noexcept {
    if (enable != isEnabled) {
        const Result result = enable
            ? CallMinHook<MH_EnableHook>(target)
            : CallMinHook<MH_DisableHook>(target);
        if (!result) {
            return result;
        }
        isEnabled = enable;
    }
    return {};
}

template <typename Ret, typename... Args>
std::expected<Ret, Error>
MMinHook<Ret, Args...>::CallOriginal(Args... args) const noexcept {
    if (original == nullptr) {
        return std::unexpected { Error::NotCreated };
    }
    using FuncType = Ret(*)(Args...);
    const auto func = reinterpret_cast<FuncType>(original);
    if constexpr (std::is_same_v<Ret, void>) {
        func(args...);
        return {};
    } else {
        return { func(args...) };
    }
}
} // namespace mmh
