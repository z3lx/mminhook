#pragma once

#include "mmh/Error.hpp"
#include "mmh/detail/Defines.hpp"

#include <expected>
#include <string_view>

namespace mmh::detail {
using VoidResult = std::expected<void, Error>;

MMH_EXTERN MMH_API VoidResult MhInitialize(
    bool initialize
) noexcept;

MMH_EXTERN MMH_API VoidResult MhCreate(
    void* target,
    void* detour,
    void*& outOriginal
) noexcept;

MMH_EXTERN MMH_API VoidResult MhCreate(
    std::wstring_view moduleName,
    std::string_view functionName,
    void* detour,
    void*& outOriginal,
    void*& outTarget
) noexcept;

MMH_EXTERN MMH_API VoidResult MhEnable(
    void* target,
    bool enable
) noexcept;

MMH_EXTERN MMH_API VoidResult MhRemove(
    void* target
) noexcept;
} // namespace mmh::detail
