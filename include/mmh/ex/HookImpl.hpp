#pragma once

#include "mmh/Defines.hpp"
#include "mmh/Hook.hpp"
#include "mmh/ex/Exception.hpp"
#include "mmh/ex/Hook.hpp"

#if !MMH_HAS_EXCEPTIONS
#include <exception>
#endif
#include <string_view>
#include <type_traits>
#include <utility>

namespace mmh::ex {
namespace detail {
template <typename Value>
Value ToException(Result<Value>&& result) {
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
Hook<Ret, Args...> Hook<Ret, Args...>::Create(
    void* target, void* detour, const bool enable) {
    Hook hook {};
    hook.hook = detail::ToException(mmh::Hook<Ret, Args...>::Create(
        target,
        detour,
        enable
    ));
    return hook;
}

template <typename Ret, typename... Args>
Hook<Ret, Args...> Hook<Ret, Args...>::Create(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    const bool enable) {
    Hook hook {};
    hook.hook = detail::ToException(mmh::Hook<Ret, Args...>::Create(
        moduleName,
        functionName,
        detour,
        enable
    ));
    return hook;
}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook() noexcept = default;

template <typename Ret, typename... Args>
Hook<Ret, Args...>::Hook(Hook&& other) noexcept
    : hook { std::move(other.hook) } {}

template <typename Ret, typename... Args>
Hook<Ret, Args...>::~Hook() noexcept = default;

template <typename Ret, typename... Args>
Hook<Ret, Args...>& Hook<Ret, Args...>::operator=(Hook&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    hook = std::move(other.hook);
    return *this;
}

template <typename Ret, typename... Args>
void* Hook<Ret, Args...>::GetTarget() const noexcept {
    return hook.GetTarget();
}

template <typename Ret, typename... Args>
void* Hook<Ret, Args...>::GetDetour() const noexcept {
    return hook.GetDetour();
}

template <typename Ret, typename... Args>
void* Hook<Ret, Args...>::GetOriginal() const noexcept {
    return hook.GetOriginal();
}

template <typename Ret, typename... Args>
bool Hook<Ret, Args...>::IsCreated() const noexcept {
    return hook.IsCreated();
}

template <typename Ret, typename... Args>
bool Hook<Ret, Args...>::IsEnabled() const noexcept {
    return hook.IsEnabled();
}

template <typename Ret, typename... Args>
void Hook<Ret, Args...>::Enable(const bool enable) {
    detail::ToException(hook.Enable(enable));
}

template <typename Ret, typename... Args>
Ret Hook<Ret, Args...>::CallOriginal(Args... args) const {
    if constexpr (std::is_void_v<Ret>) {
        detail::ToException(hook.CallOriginal(args...));
        return;
    } else {
        return detail::ToException(hook.CallOriginal(args...));
    }
}
} // namespace mmh::ex
