#pragma once

#ifndef MMH_MODULE
#include <MinHook.h>

#include <cstdint>

#define MMH_EXPORT
#else
#define MMH_EXPORT export
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
} // namespace mmh

#undef MMH_EXPORT
