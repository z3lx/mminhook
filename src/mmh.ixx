module;

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

export module mmh;

export namespace mmh {
template <typename Ret, typename... Args>
class MMinHook {
public:
    MMinHook(void* target, void* detour, bool enable = false);
    ~MMinHook() noexcept;

    MMinHook(const MMinHook& other) = delete;
    MMinHook& operator=(const MMinHook& other) = delete;
    MMinHook(MMinHook&& other) noexcept;
    MMinHook& operator=(MMinHook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable);

    Ret CallOriginal(Args... args) const;

private:
    bool isEnabled;
    void* target;
    void* original;
};
} // namespace mmh

module :private;

import std;

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
MMinHook<Ret, Args...>::MMinHook(
    void* target, void* detour, const bool enable)
    : isEnabled { enable }
    , target { target }
    , original { nullptr } {
    std::lock_guard lock { mutex };

    const auto handleMhError = [](const MH_STATUS status) {
        if (status != MH_OK) {
            MH_Uninitialize();
            --referenceCount;
            ThrowStatus(status);
        }
    };

    // Initialize MinHook
    if (referenceCount++ == 0) {
        handleMhError(MH_Initialize());
    }

    // Create and enable the hook
    handleMhError(MH_CreateHook(target, detour, &original));
    if (isEnabled) {
        handleMhError(MH_EnableHook(target));
    }
}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>::~MMinHook() noexcept {
    std::lock_guard lock { mutex };

    if (target == nullptr) {
        return;
    }

    MH_RemoveHook(target);
    if (--referenceCount == 0) {
        MH_Uninitialize();
    }
}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>::MMinHook(MMinHook&& other) noexcept
    : isEnabled { other.isEnabled }
    , target { other.target }
    , original { other.original } {
    other.isEnabled = false;
    other.target = nullptr;
    other.original = nullptr;
}

template <typename Ret, typename... Args>
MMinHook<Ret, Args...>&
MMinHook<Ret, Args...>::operator=(MMinHook&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    isEnabled = other.isEnabled;
    target = other.target;
    original = other.original;

    other.isEnabled = false;
    other.target = nullptr;
    other.original = nullptr;

    return *this;
}

template <typename Ret, typename... Args>
bool MMinHook<Ret, Args...>::IsEnabled() const noexcept {
    return isEnabled;
}

template <typename Ret, typename... Args>
void MMinHook<Ret, Args...>::Enable(const bool enable) {
    if (enable && !isEnabled) {
        ThrowOnError(MH_EnableHook(target));
    } else if (!enable && isEnabled) {
        ThrowOnError(MH_DisableHook(target));
    }
    isEnabled = enable;
}

template <typename Ret, typename... Args>
Ret MMinHook<Ret, Args...>::CallOriginal(Args... args) const {
    using FuncPtr = Ret(*)(Args...);
    return reinterpret_cast<FuncPtr>(original)(args...);
}
} // namespace mmh
