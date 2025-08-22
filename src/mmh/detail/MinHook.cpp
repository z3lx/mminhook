#include "mmh/detail/MinHook.hpp"
#include "mmh/Error.hpp"

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

#include <cstdint>
#include <expected>
#include <mutex>
#include <string_view>

namespace {
template <typename... Ignored>
mmh::detail::VoidResult ToResult(
    const MH_STATUS status,
    const Ignored... ignored) noexcept {
    if (status == MH_OK || ((status == ignored) || ...)) {
        return {};
    }
    const mmh::Error error = [status]() {
        using enum mmh::Error;
        switch (status) {
        case MH_ERROR_ALREADY_CREATED:
            return AlreadyCreated;
        case MH_ERROR_NOT_CREATED:
            return NotCreated;
        case MH_ERROR_NOT_EXECUTABLE:
            return NotExecutable;
        case MH_ERROR_UNSUPPORTED_FUNCTION:
            return UnsupportedFunction;
        case MH_ERROR_MEMORY_ALLOC:
            return MemoryAlloc;
        case MH_ERROR_MEMORY_PROTECT:
            return MemoryProtect;
        case MH_ERROR_MODULE_NOT_FOUND:
            return ModuleNotFound;
        case MH_ERROR_FUNCTION_NOT_FOUND:
            return FunctionNotFound;
        default:
            return Unknown;
        }
    }();
    return std::unexpected { error };
}
} // namespace

namespace mmh::detail {
VoidResult MhInitialize(const bool initialize) noexcept {
    static std::mutex mutex {};
    static std::size_t referenceCount = 0;
    std::lock_guard lock { mutex };
    if (initialize && referenceCount++ == 0) {
        return ToResult(MH_Initialize(), MH_ERROR_ALREADY_INITIALIZED);
    }
    if (!initialize && --referenceCount == 0) {
        return ToResult(MH_Uninitialize(), MH_ERROR_NOT_INITIALIZED);
    }
    return {};
}

VoidResult MhCreate(void* target, void* detour, void*& outOriginal) noexcept {
    return ToResult(MH_CreateHook(target, detour, &outOriginal));
}

VoidResult MhCreate(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    void*& outOriginal,
    void*& outTarget) noexcept {
    return ToResult(MH_CreateHookApiEx(
        moduleName.data(),
        functionName.data(),
        detour,
        &outOriginal,
        &outTarget
    ));
}

VoidResult MhEnable(void* target, const bool enable) noexcept {
    return enable
        ? ToResult(MH_EnableHook(target), MH_ERROR_ENABLED)
        : ToResult(MH_DisableHook(target), MH_ERROR_DISABLED);
}

VoidResult MhRemove(void* target) noexcept {
    return ToResult(MH_RemoveHook(target), MH_ERROR_NOT_CREATED);
}
} // namespace mmh::detail
