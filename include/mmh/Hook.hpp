#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"

#include <expected>

#define MMH_EXPORT
#define MMH_INLINE inline
#else
#define MMH_EXPORT export
#define MMH_INLINE
#endif

namespace mmh {
MMH_EXPORT template <typename Ret, typename... Args>
class Hook {
public:
    [[nodiscard]] static std::expected<Hook, Error>
    Create(void* target, void* detour, bool enable = false) noexcept;

    Hook() noexcept;
    ~Hook() noexcept;

    Hook(const Hook& other) = delete;
    Hook& operator=(const Hook& other) = delete;
    Hook(Hook&& other) noexcept;
    Hook& operator=(Hook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    [[nodiscard]] std::expected<void, Error>
    Enable(bool enable) noexcept;
    [[nodiscard]] std::expected<Ret, Error>
    CallOriginal(Args... args) const noexcept;

private:
    Hook(void* target, void* original, bool isEnabled) noexcept;

    void* target;
    void* original;
    bool isEnabled;
};
} // namespace mmh

#ifndef MMH_MODULE
#include "mmh/HookImpl.hpp"
#endif
