#pragma once

#ifndef MMH_MODULE
#include "mmh/Error.hpp"

#include <exception>

#define MMH_EXPORT
#else
#define MMH_EXPORT export
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

#include "mmh/ExceptionImpl.hpp"
