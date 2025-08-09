module;

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

export module mmh;

import std;

export namespace mmh {
enum class Status : std::int8_t {
    Unknown = MH_UNKNOWN,
    Ok = MH_OK,
    ErrorAlreadyInitialized = MH_ERROR_ALREADY_INITIALIZED,
    ErrorNotInitialized = MH_ERROR_NOT_INITIALIZED,
    ErrorAlreadyCreated = MH_ERROR_ALREADY_CREATED,
    ErrorNotCreated = MH_ERROR_NOT_CREATED,
    ErrorEnabled = MH_ERROR_ENABLED,
    ErrorDisabled = MH_ERROR_DISABLED,
    ErrorNotExecutable = MH_ERROR_NOT_EXECUTABLE,
    ErrorUnsupportedFunction = MH_ERROR_UNSUPPORTED_FUNCTION,
    ErrorMemoryAlloc = MH_ERROR_MEMORY_ALLOC,
    ErrorMemoryProtect = MH_ERROR_MEMORY_PROTECT,
    ErrorModuleNotFound = MH_ERROR_MODULE_NOT_FOUND,
    ErrorFunctionNotFound = MH_ERROR_FUNCTION_NOT_FOUND
};

template <typename Ret, typename... Args>
class MMinHook {
public:
    [[nodiscard]] static std::expected<MMinHook, Status>
    Create(void* target, void* detour, bool enable = false) noexcept;

    ~MMinHook() noexcept;

    MMinHook(const MMinHook& other) = delete;
    MMinHook& operator=(const MMinHook& other) = delete;
    MMinHook(MMinHook&& other) noexcept;
    MMinHook& operator=(MMinHook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    Status Enable(bool enable) noexcept;

    Ret CallOriginal(Args... args) const noexcept;

private:
    MMinHook(void* target, void* original, bool isEnabled) noexcept;

    void* target;
    void* original;
    bool isEnabled;
};
} // namespace mmh

module :private;

std::mutex mutex {};
size_t referenceCount { 0 };

void ThrowStatus(const MH_STATUS status) {
    throw std::runtime_error { MH_StatusToString(status) };
}

void ThrowOnError(const MH_STATUS status) {
    if (status != MH_OK) {
        ThrowStatus(status);
    }
}

namespace mmh {
template <typename Ret, typename... Args>
std::expected<MMinHook<Ret, Args...>, Status>
MMinHook<Ret, Args...>::Create(
    void* target, void* detour, const bool enable) noexcept {
    std::lock_guard lock { mutex };

    if (referenceCount++ == 0) {
        if (const MH_STATUS status = MH_Initialize();
            status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED) {
            return std::unexpected { static_cast<Status>(status) };
        }
    }

    using StatusResult = std::expected<void, Status>;

    void* original = nullptr;
    const auto createHook = [=, &original]() -> StatusResult {
        if (const MH_STATUS status = MH_CreateHook(target, detour, &original);
            status != MH_OK) {
            return std::unexpected { static_cast<Status>(status) };
        }
        return {};
    };

    const auto enableHook = [=]() -> StatusResult {
        if (enable) {
            if (const MH_STATUS status = MH_EnableHook(target);
                status != MH_OK) {
                return std::unexpected { static_cast<Status>(status) };
            }
        }
        return {};
    };

    if (const StatusResult result = createHook().and_then(enableHook);
        !result.has_value()) {
        if (--referenceCount == 0) {
            MH_Uninitialize();
        }
        return std::unexpected { result.error() };
    }

    return MMinHook { target, original, enable };
}

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
    std::lock_guard lock { mutex };
    if (--referenceCount == 0) {
        MH_Uninitialize();
    }
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
Status MMinHook<Ret, Args...>::Enable(const bool enable) noexcept {
    MH_STATUS status = MH_OK;
    if (enable && !isEnabled) {
        status = MH_EnableHook(target);
    } else if (!enable && isEnabled) {
        status = MH_DisableHook(target);
    }
    isEnabled = enable;
    return static_cast<Status>(status);
}

template <typename Ret, typename... Args>
Ret MMinHook<Ret, Args...>::CallOriginal(Args... args) const noexcept {
    using FuncPtr = Ret(*)(Args...);
    return reinterpret_cast<FuncPtr>(original)(args...);
}
} // namespace mmh
