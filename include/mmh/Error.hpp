#pragma once

#include "mmh/detail/Defines.hpp"

#include <cstdint>

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
} // namespace mmh
