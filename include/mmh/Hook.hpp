#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"
#include "mmh/Exception.hpp"

#include <expected>
#include <string_view>

#define MMH_EXPORT
#else
#define MMH_EXPORT export
#endif

namespace mmh {
MMH_EXPORT template <typename Value>
using Result = std::expected<Value, Error>;

MMH_EXPORT template <typename Ret, typename... Args>
class Hook {
public:
    [[nodiscard]] static Result<Hook> TryCreate(
        void* target,
        void* detour,
        bool enable = false
    ) noexcept;
    [[nodiscard]] static Hook Create(
        void* target,
        void* detour,
        bool enable = false
    );

    [[nodiscard]] static Result<Hook> TryCreate(
        std::wstring_view moduleName,
        std::string_view functionName,
        void* detour,
        bool enable = false
    ) noexcept;
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
    [[nodiscard]] Result<void> TryEnable(bool enable) noexcept;
    void Enable(bool enable);

    [[nodiscard]] Result<Ret> TryCallOriginal(Args... args) const noexcept;
    Ret CallOriginal(Args... args) const;

private:
    template <typename CreateHookCallable>
    static Result<Hook> TryCreateImpl(
        CreateHookCallable createHookCallable,
        bool enable
    ) noexcept;

    void* target;
    void* original;
    bool isEnabled;
};
} // namespace mmh

#include "mmh/HookImpl.hpp"
