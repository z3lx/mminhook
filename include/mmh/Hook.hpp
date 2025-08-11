#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"

#include <expected>

#define MMH_EXPORT
#else
#define MMH_EXPORT export
#endif

namespace mmh {
MMH_EXPORT template <typename Ret, typename... Args>
class Hook {
public:
    [[nodiscard]] static std::expected<Hook, Error>
    TryCreate(void* target, void* detour, bool enable = false) noexcept;
    static Hook Create(void* target, void* detour, bool enable = false);

    Hook() noexcept;
    ~Hook() noexcept;

    Hook(const Hook& other) = delete;
    Hook& operator=(const Hook& other) = delete;
    Hook(Hook&& other) noexcept;
    Hook& operator=(Hook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    [[nodiscard]] std::expected<void, Error>
    TryEnable(bool enable) noexcept;
    void Enable(bool enable);

    [[nodiscard]] std::expected<Ret, Error>
    TryCallOriginal(Args... args) const noexcept;
    Ret CallOriginal(Args... args) const;

private:
    void* target;
    void* original;
    bool isEnabled;
};
} // namespace mmh

#include "mmh/HookImpl.hpp"
