#ifndef SDLRAII_COMPAT_MACROS_INCLUDE_GUARDD
#define SDLRAII_COMPAT_MACROS_INCLUDE_GUARDD

#include "hedley.h"

// clang-format off
#define SDLRAII_LIKELY(...)   HEDLEY_LIKELY(__VA_ARGS__)
#define SDLRAII_UNLIKELY(...) HEDLEY_UNLIKELY(__VA_ARGS__)

#define SDLRAII_HOT_IF(...)  if(SDLRAII_LIKELY(__VA_ARGS__))
#define SDLRAII_COLD_IF(...) if(SDLRAII_UNLIKELY(__VA_ARGS__))
// clang-format! on

#define SDLRAII_BODY_EXP(...)                                                  \
  noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) { return __VA_ARGS__; }
#define SDLRAII_DECL_RET(...)                                                  \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

#define SDLRAII_FWD(name) std::forward<decltype(name)>(name)
#define SDLRAII_LIFT(name)                                                     \
  [](auto&&... args) SDLRAII_BODY_EXP(name(SDLRAII_FWD(args)...))

#endif // SDLRAII_COMPAT_MACROS_INCLUDE_GUARDD
