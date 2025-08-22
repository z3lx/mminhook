#include "mmh/Error.hpp"

#include <string_view>

#define CASE_RETURN_STR(x) case x: return #x;

namespace mmh {
std::string_view ToString(const Error error) noexcept {
    using enum Error;
    switch (error) {
    CASE_RETURN_STR(Unknown)
    CASE_RETURN_STR(AlreadyCreated)
    CASE_RETURN_STR(NotCreated)
    CASE_RETURN_STR(NotExecutable)
    CASE_RETURN_STR(UnsupportedFunction)
    CASE_RETURN_STR(MemoryAlloc)
    CASE_RETURN_STR(MemoryProtect)
    CASE_RETURN_STR(ModuleNotFound)
    CASE_RETURN_STR(FunctionNotFound)
    default: return "Unknown";
    }
}
} // namespace mmh
