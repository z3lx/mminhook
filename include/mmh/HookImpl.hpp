#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"
#include "mmh/Hook.hpp"

#include <MinHook.h>

#include <cstdint>
#include <expected>
#include <mutex>
#include <string_view>
#include <type_traits>

#define MMH_INLINE inline
#else
#define MMH_INLINE
#endif

namespace mmh {
namespace detail {
MMH_INLINE Result<void> ToResult(const MH_STATUS status) noexcept {
    if (status != MH_OK) {
        return std::unexpected { static_cast<Error>(status) };
    }
    return {};
}

MMH_INLINE Result<void> InitializeMinHook(const bool initialize) noexcept {
    static std::mutex mutex {};
    static std::size_t referenceCount = 0;
    std::lock_guard lock { mutex };
    constexpr Result<void> success {};
    return initialize
        ? (referenceCount++ == 0 ? ToResult(MH_Initialize()) : success)
        : (--referenceCount == 0 ? ToResult(MH_Uninitialize()) : success);
}
} // namespace detail

template <typename Ret, typename... Args>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::Create(
    void* target, void* detour, const bool enable) noexcept {
    const auto createHook = [=](
        void*& outTarget, void*& outOriginal) -> Result<void> {
        outTarget = target;
        return detail::ToResult(MH_CreateHook(
            target,
            detour,
            &outOriginal
        ));
    };
    return TryCreateImpl(createHook, enable);
}

template <typename Ret, typename... Args>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::Create(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    const bool enable) noexcept {
    const auto createHook = [=](
        void*& outTarget, void*& outOriginal) -> Result<void> {
        return detail::ToResult(MH_CreateHookApiEx(
            moduleName.data(),
            functionName.data(),
            detour,
            &outOriginal,
            &outTarget
        ));
    };
    return TryCreateImpl(createHook, enable);
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook() noexcept
    : target { nullptr }, original { nullptr }, isEnabled { false } {};

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
Hook<Ret, Args...>::~Hook() noexcept {
    if (target == nullptr) {
        return;
    }
    MH_RemoveHook(target);
    detail::InitializeMinHook(false);
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>& Hook<Ret, Args...>::operator=(Hook&& other) noexcept {
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
Result<void> Hook<Ret, Args...>::Enable(
    const bool enable) noexcept {
    if (enable != isEnabled) {
        const Result<void> result = enable
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
Result<Ret> Hook<Ret, Args...>::CallOriginal(Args... args) const noexcept {
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
template <typename CreateHookCallable>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::TryCreateImpl(
    CreateHookCallable createHookCallable,
    const bool enable) noexcept {
    void* target = nullptr;
    void* original = nullptr;
    const auto createHook = [&]() -> Result<void> {
        return createHookCallable(target, original);
    };
    const auto enableHook = [=]() -> Result<void> {
        return enable ?
            detail::ToResult(MH_EnableHook(target)) :
            Result<void> {};
    };
    const Result<void> result = detail::InitializeMinHook(true)
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
} // namespace mmh

#undef MMH_EXPORT
#undef MMH_INLINE
