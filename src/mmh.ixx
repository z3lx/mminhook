module;

#include <cstdint>
#include <exception>
#include <expected>
#include <string_view>
#include <type_traits>
#include <utility>

#define MMH_BUILD_MODULE

export module mmh;

#include "mmh/Error.hpp"
#include "mmh/Exception.hpp"
#include "mmh/Hook.hpp"
