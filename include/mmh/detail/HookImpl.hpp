#pragma once

#include "mmh/Error.hpp"
#include "mmh/Exception.hpp"
#include "mmh/Hook.hpp"
#include "mmh/detail/Defines.hpp"
#include "mmh/detail/MinHook.hpp"

#if !MMH_HAS_EXCEPTIONS
#include <exception>
#endif
#include <expected>
#include <string_view>
#include <type_traits>
#include <utility>

namespace mmh {
namespace detail {
template <typename Value>
Value ValueOrThrow(Result<Value>&& result) {
    if (!result) {
#if MMH_HAS_EXCEPTIONS
        throw Exception { result.error() };
#else
        std::terminate();
#endif
    }
    if constexpr (std::is_void_v<Value>) {
        return;
    } else {
        return std::move(result.value());
    }
}
} // namespace detail

template <typename Ret, typename... Args>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::TryCreate(
    void* target,
    void* detour,
    const bool enable) noexcept {
    Hook hook {};
    hook.target = target;
    hook.detour = detour;
    hook.isEnabled = enable;
    Result<void> result = detail::MhCreate(
        hook.target,
        hook.detour,
        hook.original,
        hook.isEnabled
    );
    return result
        ? Result<Hook> { std::move(hook) }
        : std::unexpected { result.error() };
}

template <typename Ret, typename... Args>
Hook<Ret, Args...> Hook<Ret, Args...>::Create(
    void* target,
    void* detour,
    const bool enable) {
    return detail::ValueOrThrow(Hook::TryCreate(
        target,
        detour,
        enable
    ));
}

template <typename Ret, typename... Args>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::TryCreate(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    const bool enable) noexcept {
    Hook hook {};
    hook.detour = detour;
    hook.isEnabled = enable;
    Result<void> result = detail::MhCreate(
        moduleName,
        functionName,
        hook.detour,
        hook.original,
        hook.target,
        hook.isEnabled
    );
    return result
        ? Result<Hook> { std::move(hook) }
        : std::unexpected { result.error() };
}

template <typename Ret, typename... Args>
Hook<Ret, Args...> Hook<Ret, Args...>::Create(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    const bool enable) {
    return detail::ValueOrThrow(Hook::TryCreate(
        moduleName,
        functionName,
        detour,
        enable
    ));
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook() noexcept
    : target { nullptr }
    , detour { nullptr }
    , original { nullptr }
    , isEnabled { false } {};

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook(Hook&& other) noexcept
    : target { other.target }
    , detour { other.detour }
    , original { other.original }
    , isEnabled { other.isEnabled } {
    other.target = nullptr;
    other.detour = nullptr;
    other.original = nullptr;
    other.isEnabled = false;
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::~Hook() noexcept {
    detail::MhRemove(target);
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>& Hook<Ret, Args...>::operator=(Hook&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    detail::MhRemove(target);
    target = other.target;
    original = other.original;
    isEnabled = other.isEnabled;
    other.target = nullptr;
    other.original = nullptr;
    other.isEnabled = false;
    return *this;
}

template <typename Ret, typename... Args>
void* Hook<Ret, Args...>::GetTarget() const noexcept {
    return target;
}

template <typename Ret, typename... Args>
void* Hook<Ret, Args...>::GetDetour() const noexcept {
    return detour;
}

template <typename Ret, typename... Args>
void* Hook<Ret, Args...>::GetOriginal() const noexcept {
    return original;
}

template <typename Ret, typename... Args>
bool Hook<Ret, Args...>::IsCreated() const noexcept {
    return original != nullptr;
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::operator bool() const noexcept {
    return IsCreated();
}

template <typename Ret, typename... Args>
bool Hook<Ret, Args...>::IsEnabled() const noexcept {
    return isEnabled;
}

template <typename Ret, typename... Args>
Result<void> Hook<Ret, Args...>::TryEnable(
    const bool enable) noexcept {
    if (enable != isEnabled) {
        if (const Result<void> result = detail::MhEnable(target, enable);
            !result) {
            return result;
        }
        isEnabled = enable;
    }
    return {};
}

template <typename Ret, typename... Args>
void Hook<Ret, Args...>::Enable(const bool enable) {
    detail::ValueOrThrow(TryEnable(enable));
}

template <typename Ret, typename... Args>
Result<Ret> Hook<Ret, Args...>::TryCallOriginal(Args... args) const noexcept {
    if (!IsCreated()) {
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
        detail::ValueOrThrow(TryCallOriginal(args...));
        return;
    } else {
        return detail::ValueOrThrow(TryCallOriginal(args...));
    }
}
} // namespace mmh
