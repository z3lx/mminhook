#pragma once

#include "mmh/Error.hpp"
#include "mmh/detail/Defines.hpp"

#include <expected>
#include <string_view>

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

    [[nodiscard]] void* GetTarget() const noexcept;
    [[nodiscard]] void* GetDetour() const noexcept;
    [[nodiscard]] void* GetOriginal() const noexcept;

    [[nodiscard]] bool IsCreated() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    [[nodiscard]] Result<void> TryEnable(bool enable) noexcept;
    void Enable(bool enable);

    [[nodiscard]] Result<Ret> TryCallOriginal(Args... args) const noexcept;
    Ret CallOriginal(Args... args) const;

private:
    void* target;
    void* detour;
    void* original;
    bool isEnabled;
};
} // namespace mmh

#include "mmh/detail/HookImpl.hpp"
