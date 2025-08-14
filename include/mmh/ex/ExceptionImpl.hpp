#pragma once

#include "mmh/Error.hpp"
#include "mmh/ex/Exception.hpp"

#include <MinHook.h>

namespace mmh {
inline Exception::Exception(const Error error) noexcept
    : error { error } {}

inline Exception::~Exception() noexcept = default;

inline Error Exception::GetError() const noexcept {
    return error;
}

inline const char* Exception::what() const noexcept {
    return MH_StatusToString(static_cast<MH_STATUS>(error));
}
} // namespace mmh
