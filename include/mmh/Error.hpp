#pragma once

#include "mmh/detail/Defines.hpp"

#include <cstdint>
#include <string_view>

namespace mmh {
MMH_EXPORT enum class Error : std::uint8_t {
    Unknown,
    AlreadyCreated,
    NotCreated,
    NotExecutable,
    UnsupportedFunction,
    MemoryAlloc,
    MemoryProtect,
    ModuleNotFound,
    FunctionNotFound
};

MMH_EXPORT MMH_EXTERN MMH_API std::string_view ToString(Error error) noexcept;
} // namespace mmh
