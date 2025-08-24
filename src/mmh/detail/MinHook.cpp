#include "mmh/detail/MinHook.hpp"
#include "mmh/Error.hpp"

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

#include <cstdint>
#include <expected>
#include <mutex>
#include <string_view>

namespace {
using namespace mmh;
using namespace mmh::detail;

template <typename... Ignored>
VoidResult ToResult(
    const MH_STATUS status,
    const Ignored... ignored) noexcept {
    if (status == MH_OK || ((status == ignored) || ...)) {
        return {};
    }
    const Error error = [status]() {
        using enum Error;
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

VoidResult MhInitialize(const bool initialize) noexcept try {
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
} catch (...) {
    return std::unexpected { Error::Unknown };
}

template <typename CreateHookCallable>
VoidResult MhCreateImpl(
    CreateHookCallable createHook,
    void*& target,
    const bool enable) noexcept {
    const auto enableHook = [=, &target]() {
        return MhEnable(target, enable);
    };
    const VoidResult result = MhInitialize(true)
        .and_then(createHook)
        .and_then(enableHook);
    if (!result) {
        MhInitialize(false);
    }
    return result;
}
} // namespace

namespace mmh::detail {
VoidResult MhCreate(
    void* target,
    void* detour,
    void*& outOriginal,
    const bool enable) noexcept {
    const auto createHook = [=, &outOriginal]() {
        return ToResult(MH_CreateHook(
            target,
            detour,
            &outOriginal
        ));
    };
    return MhCreateImpl(createHook, target, enable);
}

VoidResult MhCreate(
    const std::wstring_view moduleName,
    const std::string_view functionName,
    void* detour,
    void*& outOriginal,
    void*& outTarget,
    const bool enable) noexcept {
    const auto createHook = [=, &outOriginal, &outTarget]() {
        return ToResult(MH_CreateHookApiEx(
            moduleName.data(),
            functionName.data(),
            detour,
            &outOriginal,
            &outTarget
        ));
    };
    return MhCreateImpl(createHook, outTarget, enable);
}

VoidResult MhEnable(void* target, const bool enable) noexcept {
    return enable
        ? ToResult(MH_EnableHook(target), MH_ERROR_ENABLED)
        : ToResult(MH_DisableHook(target), MH_ERROR_DISABLED);
}

VoidResult MhRemove(void* target) noexcept {
    if (target == nullptr) {
        return std::unexpected { Error::NotCreated };
    }
    return ToResult(MH_RemoveHook(target)).and_then([]() {
        return MhInitialize(false);
    });
}
} // namespace mmh::detail
