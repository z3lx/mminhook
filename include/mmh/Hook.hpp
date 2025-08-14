#pragma once

#include "mmh/Error.hpp"

#include <expected>
#include <string_view>

#ifndef MMH_EXPORT
#define MMH_EXPORT
#endif

namespace mmh {
MMH_EXPORT template <typename Value>
using Result = std::expected<Value, Error>;

MMH_EXPORT template <typename Ret, typename... Args>
class Hook {
public:
    [[nodiscard]] static Result<Hook> Create(
        void* target,
        void* detour,
        bool enable = false
    ) noexcept;
    [[nodiscard]] static Result<Hook> Create(
        std::wstring_view moduleName,
        std::string_view functionName,
        void* detour,
        bool enable = false
    ) noexcept;

    Hook() noexcept;
    Hook(Hook&& other) noexcept;
    Hook(const Hook&) = delete;

    ~Hook() noexcept;

    Hook& operator=(Hook&& other) noexcept;
    Hook& operator=(const Hook&) = delete;

    [[nodiscard]] void* GetTarget() const noexcept;
    [[nodiscard]] void* GetDetour() const noexcept;
    [[nodiscard]] void* GetOriginal() const noexcept;

    [[nodiscard]] bool IsCreated() const noexcept;
    [[nodiscard]] bool IsEnabled() const noexcept;
    [[nodiscard]] Result<void> Enable(bool enable) noexcept;

    [[nodiscard]] Result<Ret> CallOriginal(Args... args) const noexcept;

private:
    void* target;
    void* detour;
    void* original;
    bool isEnabled;
};
} // namespace mmh

#include "mmh/HookImpl.hpp"
