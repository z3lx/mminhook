#pragma once

#include "mmh/Defines.hpp"
#include "mmh/Error.hpp"
#include "mmh/Hook.hpp"

#include <MinHook.h>

#include <expected>
#include <string_view>
#include <type_traits>

namespace mmh {
namespace detail {
MMH_EXTERN Result<void> ToResult(MH_STATUS status) noexcept;
MMH_EXTERN Result<void> InitializeMinHook(bool initialize) noexcept;

template <typename Ret, typename... Args, typename CreateHookCallable>
static Result<Hook<Ret, Args...>> CreateImpl(
    CreateHookCallable createHookCallable,
    const bool enable) noexcept {
    Hook<Ret, Args...> hook {};
    const auto createHook = [&]() -> Result<void> {
        return createHookCallable(hook);
    };
    const auto enableHook = [&]() -> Result<void> {
        return hook.Enable(enable);
    };
    const Result<void> result = InitializeMinHook(true)
        .and_then(createHook)
        .and_then(enableHook);
    if (!result) {
        InitializeMinHook(false);
        return std::unexpected { result.error() };
    }
    return hook;
}
} // namespace detail

template <typename Ret, typename... Args>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::Create(
    void* target, void* detour, const bool enable) noexcept {
    const auto createHook = [=](Hook& hook) -> Result<void> {
        hook.target = target;
        hook.detour = detour;
        return detail::ToResult(MH_CreateHook(
            hook.target,
            hook.detour,
            &hook.original
        ));
    };
    return detail::CreateImpl<Ret, Args...>(createHook, enable);
}

template <typename Ret, typename... Args>
Result<Hook<Ret, Args...>> Hook<Ret, Args...>::Create(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    const bool enable) noexcept {
    const auto createHook = [=](Hook& hook) -> Result<void> {
        hook.detour = detour;
        return detail::ToResult(MH_CreateHookApiEx(
            moduleName.data(),
            functionName.data(),
            hook.detour,
            &hook.original,
            &hook.target
        ));
    };
    return detail::CreateImpl<Ret, Args...>(createHook, enable);
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
    return target != nullptr;
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
} // namespace mmh
