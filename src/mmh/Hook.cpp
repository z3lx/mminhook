#include "mmh/Hook.hpp"
#include "mmh/Error.hpp"

#include <cstdint>
#include <expected>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

namespace mmh::detail {
Result<void> ToResult(const MH_STATUS status) noexcept {
    if (status != MH_OK) {
        return std::unexpected { static_cast<Error>(status) };
    }
    return {};
}

Result<void> InitializeMinHook(const bool initialize) noexcept {
    static std::mutex mutex {};
    static std::size_t referenceCount = 0;
    std::lock_guard lock { mutex };
    constexpr Result<void> success {};
    return initialize
        ? (referenceCount++ == 0 ? ToResult(MH_Initialize()) : success)
        : (--referenceCount == 0 ? ToResult(MH_Uninitialize()) : success);
}
} // namespace mmh::detail
