#pragma once

#include "mmh/Error.hpp"
#include "mmh/ex/Exception.hpp"

#include <MinHook.h>

#ifdef MMH_MODULE
#define MMH_INLINE
#else
#define MMH_INLINE inline
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

#undef MMH_INLINE
