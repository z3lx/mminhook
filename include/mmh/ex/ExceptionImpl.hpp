#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"
#include "mmh/ex/Exception.hpp"

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

#define MMH_INLINE inline
#else
#define MMH_INLINE
#endif

namespace mmh {
MMH_INLINE Exception::Exception(const Error error) noexcept
    : error { error } {}

MMH_INLINE Exception::~Exception() noexcept {};

MMH_INLINE Error Exception::GetError() const noexcept {
    return error;
}

MMH_INLINE const char* Exception::what() const noexcept {
    return MH_StatusToString(static_cast<MH_STATUS>(error));
}
} // namespace mmh

#undef MMH_EXPORT
