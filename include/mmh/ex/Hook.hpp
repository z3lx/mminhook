#pragma once

#ifndef MMH_MODULE
#include "mmh/Hook.hpp"

#include <string_view>

#define MMH_EXPORT
#else
#define MMH_EXPORT export
#endif

namespace mmh::ex {
MMH_EXPORT template <typename Ret, typename... Args>
class Hook {
public:
    [[nodiscard]] static Hook Create(
        void* target,
        void* detour,
        bool enable = false
    );
    [[nodiscard]] static Hook Create(
        std::wstring_view moduleName,
        std::string_view functionName,
        void* detour,
        bool enable = false
    );

    Hook() noexcept;
    Hook(Hook&& other) noexcept;
    Hook(const Hook&) = delete;

    ~Hook() noexcept;

    Hook& operator=(Hook&& other) noexcept;
    Hook& operator=(const Hook&) = delete;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable);

    Ret CallOriginal(Args... args) const;

private:
    mmh::Hook<Ret, Args...> hook;
};
} // namespace mmh::ex

#include "mmh/ex/HookImpl.hpp"
