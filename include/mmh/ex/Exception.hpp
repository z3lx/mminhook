#pragma once

#include "mmh/Error.hpp"

#include <exception>

#ifdef MMH_MODULE
#define MMH_EXPORT export
#else
#define MMH_EXPORT
#endif

namespace mmh {
MMH_EXPORT class Exception final : public std::exception {
public:
    explicit Exception(Error error) noexcept;
    ~Exception() noexcept override;

    [[nodiscard]] Error GetError() const noexcept;
    [[nodiscard]] const char* what() const noexcept override;

private:
    Error error;
};
} // namespace mmh

#undef MMH_EXPORT

#include "mmh/ex/ExceptionImpl.hpp"
