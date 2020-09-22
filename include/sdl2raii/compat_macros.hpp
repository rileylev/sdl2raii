#ifndef SDLRAII_COMPAT_MACROS_INCLUDE_GUARDD
#define SDLRAII_COMPAT_MACROS_INCLUDE_GUARDD

#if defined(__GNUC__) || defined(__clang__)
#  define SDLRAII_LIKELY(x) __builtin_expect(!!(x), 1)
#  define SDLRAII_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#  define SDLRAII_LIKELY(X) X
#  define SDLRAII_UNLIKELY(X) X
#endif

#define SDLRAII_COLD_IF(...) if(SDLRAII_UNLIKELY(__VA_ARGS__))

#define SDLRAII_BODY_EXP(...)                                                  \
  noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) { return __VA_ARGS__; }
#define SDLRAII_DECL_RET(...)                                                  \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

#endif // SDLRAII_COMPAT_MACROS_INCLUDE_GUARDD
