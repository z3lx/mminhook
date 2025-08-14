module;

#include <cstdint>
#include <exception>
#include <expected>
#include <mutex>
#include <string_view>
#include <type_traits>
#include <utility>

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

#define MMH_EXPORT export

export module mmh;

#include "mmh/Error.hpp"
#include "mmh/Hook.hpp"
#include "mmh/ex/Exception.hpp"
#include "mmh/ex/Hook.hpp"
