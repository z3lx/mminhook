#include "mmh/Exception.hpp"
#include "mmh/Error.hpp"

namespace mmh {
Exception::Exception(const Error error) noexcept
    : error { error } {}

Exception::~Exception() noexcept = default;

Error Exception::GetError() const noexcept {
    return error;
}

const char* Exception::what() const noexcept {
    return ToString(error).data();
}
} // namespace mmh
