#pragma once

#include "mmh/Error.hpp"
#include "mmh/detail/Defines.hpp"

#include <exception>

namespace mmh {
MMH_EXPORT MMH_EXTERN class MMH_API Exception final : public std::exception {
public:
    explicit Exception(Error error) noexcept;
    ~Exception() noexcept override;

    [[nodiscard]] Error GetError() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;

private:
    Error error;
};
} // namespace mmh
