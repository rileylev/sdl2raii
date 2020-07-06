#pragma once

#define SDLRAII_LIKELY(x) __builtin_expect(!!(x),1)
#define SDLRAII_UNLIKELY(x) __builtin_expect(!!(x),0)
