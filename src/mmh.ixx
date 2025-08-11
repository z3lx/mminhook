module;

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

#define MMH_MODULE

export module mmh;

import std;

#include "mmh/Error.hpp"
#include "mmh/Exception.hpp"
#include "mmh/Hook.hpp"
