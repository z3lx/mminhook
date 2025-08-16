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
#define CASE_RETURN_STR(x) case x: return #x;
    switch (error) {
    CASE_RETURN_STR(Error::Unknown)
    CASE_RETURN_STR(Error::AlreadyCreated)
    CASE_RETURN_STR(Error::NotCreated)
    CASE_RETURN_STR(Error::NotExecutable)
    CASE_RETURN_STR(Error::UnsupportedFunction)
    CASE_RETURN_STR(Error::MemoryAlloc)
    CASE_RETURN_STR(Error::MemoryProtect)
    CASE_RETURN_STR(Error::ModuleNotFound)
    CASE_RETURN_STR(Error::FunctionNotFound)
    default: return "Unknown";
    }
#undef CASE_RETURN_STR
}
} // namespace mmh
