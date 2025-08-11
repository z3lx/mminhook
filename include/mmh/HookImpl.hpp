#pragma once

#ifndef MMH_MODULE
#include "mmh/Exception.hpp"
#include "mmh/Error.hpp"
#include "mmh/Hook.hpp"

#include <MinHook.h>

#include <cstdint>
#include <expected>
#include <mutex>
#include <type_traits>
#include <utility>

#define MMH_INLINE inline
#else
#define MMH_INLINE
#endif

namespace mmh {
namespace detail {
using VoidResult = std::expected<void, Error>;
MMH_INLINE VoidResult ToResult(const MH_STATUS status) noexcept {
    if (status != MH_OK) {
        return std::unexpected { static_cast<Error>(status) };
    }
    return {};
}

template <typename V>
V ToException(std::expected<V, Error>&& result) {
    if (!result) {
        throw Exception { result.error() };
    }
    if constexpr (std::is_void_v<V>) {
        return;
    } else {
        return std::move(result.value());
    }
}

MMH_INLINE VoidResult InitializeMinHook(const bool initialize) noexcept {
    static std::mutex mutex {};
    static std::size_t referenceCount = 0;
    std::lock_guard lock { mutex };
    return initialize
        ? (referenceCount++ == 0 ? ToResult(MH_Initialize()) : VoidResult {})
        : (--referenceCount == 0 ? ToResult(MH_Uninitialize()) : VoidResult {});
}
} // namespace detail

template <typename Ret, typename... Args>
std::expected<Hook<Ret, Args...>, Error>
Hook<Ret, Args...>::TryCreate(
    void* target, void* detour, const bool enable) noexcept {
    void* original = nullptr;
    const auto createHook = [=, &original]() -> detail::VoidResult {
        return detail::ToResult(MH_CreateHook(
            target, detour, &original
        ));
    };
    const auto enableHook = [=]() -> detail::VoidResult {
        return enable
            ? detail::ToResult(MH_EnableHook(target))
            : detail::VoidResult {};
    };
    const detail::VoidResult result = detail::InitializeMinHook(true)
        .and_then(createHook)
        .and_then(enableHook);
    if (!result) {
        detail::InitializeMinHook(false);
        return std::unexpected { result.error() };
    }
    Hook hook {};
    hook.target = target;
    hook.original = original;
    hook.isEnabled = enable;
    return hook;
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>
Hook<Ret, Args...>::Create(void* target, void* detour, const bool enable) {
    return detail::ToException(TryCreate(target, detour, enable));
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook() noexcept
    : target { nullptr }, original { nullptr }, isEnabled { false } {};

template <typename Ret, typename... Args>
Hook<Ret, Args...>::~Hook() noexcept {
    if (target == nullptr) {
        return;
    }
    MH_RemoveHook(target);
    detail::InitializeMinHook(false);
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook(Hook&& other) noexcept
    : target { other.target }
    , original { other.original }
    , isEnabled { other.isEnabled } {
    other.target = nullptr;
    other.original = nullptr;
    other.isEnabled = false;
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>&
Hook<Ret, Args...>::operator=(Hook&& other) noexcept {
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
bool Hook<Ret, Args...>::IsEnabled() const noexcept {
    return isEnabled;
}

template <typename Ret, typename... Args>
std::expected<void, Error>
Hook<Ret, Args...>::TryEnable(const bool enable) noexcept {
    if (enable != isEnabled) {
        const detail::VoidResult result = enable
            ? detail::ToResult(MH_EnableHook(target))
            : detail::ToResult(MH_DisableHook(target));
        if (!result) {
            return result;
        }
        isEnabled = enable;
    }
    return {};
}

template <typename Ret, typename... Args>
void Hook<Ret, Args...>::Enable(const bool enable) {
    detail::ToException(TryEnable(enable));
}

template <typename Ret, typename... Args>
std::expected<Ret, Error>
Hook<Ret, Args...>::TryCallOriginal(Args... args) const noexcept {
    if (original == nullptr) {
        return std::unexpected { Error::NotCreated };
    }
    using FuncType = Ret(*)(Args...);
    const auto func = reinterpret_cast<FuncType>(original);
    if constexpr (std::is_void_v<Ret>) {
        func(args...);
        return {};
    } else {
        return { func(args...) };
    }
}

template <typename Ret, typename... Args>
Ret Hook<Ret, Args...>::CallOriginal(Args... args) const {
    if constexpr (std::is_void_v<Ret>) {
        detail::ToException(TryCallOriginal(args...));
    } else {
        return detail::ToException(TryCallOriginal(args...));
    }
}
} // namespace mmh

#undef MMH_EXPORT
#undef MMH_INLINE
