#pragma once

#ifndef MMH_MODULE
#include <cstdint>
#include <expected>

#include <MinHook.h>

#define MMH_EXPORT
#define MMH_INLINE inline
#else
#define MMH_EXPORT export
#define MMH_INLINE
#endif

namespace mmh {
MMH_EXPORT enum class Error : std::int8_t {
    Unknown = MH_UNKNOWN,
    AlreadyCreated = MH_ERROR_ALREADY_CREATED,
    NotCreated = MH_ERROR_NOT_CREATED,
    NotExecutable = MH_ERROR_NOT_EXECUTABLE,
    UnsupportedFunction = MH_ERROR_UNSUPPORTED_FUNCTION,
    MemoryAlloc = MH_ERROR_MEMORY_ALLOC,
    MemoryProtect = MH_ERROR_MEMORY_PROTECT,
    ModuleNotFound = MH_ERROR_MODULE_NOT_FOUND,
    FunctionNotFound = MH_ERROR_FUNCTION_NOT_FOUND
};

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
