#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"
#include "mmh/Exception.hpp"

#include <MinHook.h>
#endif

#define MMH_INLINE inline

namespace mmh {
MMH_INLINE Exception::Exception(const Error error) noexcept
    : error { error } {}

MMH_INLINE Exception::~Exception() noexcept = default;

MMH_INLINE Error Exception::GetError() const noexcept {
    return error;
}

MMH_INLINE const char* Exception::what() const noexcept {
    return MH_StatusToString(static_cast<MH_STATUS>(error));
}
} // namespace mmh

#undef MMH_EXPORT
#undef MMH_INLINE
