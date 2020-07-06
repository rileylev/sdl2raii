#pragma once

#define SDL2RAII_LIKELY(x) __builtin_expect(!!(x),1)
#define SDL2RAII_UNLIKELY(x) __builtin_expect(!!(x),0)
