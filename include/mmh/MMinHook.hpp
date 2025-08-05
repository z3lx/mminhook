#pragma once

namespace mmh {
template <typename Ret, typename... Args>
class MMinHook {
public:
    MMinHook(void* target, void* detour, bool enable = false);
    ~MMinHook() noexcept;

    MMinHook(const MMinHook& other) = delete;
    MMinHook& operator=(const MMinHook& other) = delete;
    MMinHook(MMinHook&& other) noexcept;
    MMinHook& operator=(MMinHook&& other) noexcept;

    [[nodiscard]] bool IsEnabled() const noexcept;
    void Enable(bool enable);

    Ret CallOriginal(Args... args) const;

private:
    bool isEnabled;
    void* target;
    void* original;
};
} // namespace mmh

#include "mmh/MMinHookInl.hpp"
