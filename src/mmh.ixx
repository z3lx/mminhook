module;

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

#define MMH_MODULE

export module mmh;

import std;

#include "mmh/Error.hpp"
#include "mmh/Hook.hpp"
#include "mmh/ex/Exception.hpp"
#include "mmh/ex/Hook.hpp"

module :private;

#include "mmh/ex/Exception.cpp"
