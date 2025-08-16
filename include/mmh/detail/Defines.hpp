#pragma once

#include "mmh/detail/GeneratedDefines.hpp"

#ifdef MMH_BUILD_MODULE
#define MMH_EXPORT export
#define MMH_EXTERN extern "C++"
#else
#define MMH_EXPORT
#define MMH_EXTERN
#endif

#if __cpp_exceptions == 199711
#define MMH_HAS_EXCEPTIONS 1
#else
#define MMH_HAS_EXCEPTIONS 0
#endif
