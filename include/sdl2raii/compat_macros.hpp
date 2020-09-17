#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define SDLRAII_LIKELY(x) __builtin_expect(!!(x),1)
#define SDLRAII_UNLIKELY(x) __builtin_expect(!!(x),0)
#else
#define SDLRAII_LIKELY(X) X
#define SDLRAII_UNLIKELY(X) X
#endif
