#ifndef MMH_MODULE
#include "mmh/ex/Exception.hpp"
#include "mmh/Error.hpp"

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>
#endif

namespace mmh {
Exception::Exception(const Error error) noexcept
    : error { error } {}

Exception::~Exception() noexcept {};

Error Exception::GetError() const noexcept {
    return error;
}

const char* Exception::what() const noexcept {
    return MH_StatusToString(static_cast<MH_STATUS>(error));
}
} // namespace mmh
