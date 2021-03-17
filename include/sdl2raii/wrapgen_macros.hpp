#ifndef SDLRAII_WRAPGEN_MACROS_INCLUDE_GUARD
#define SDLRAII_WRAPGEN_MACROS_INCLUDE_GUARD


#include "compat_macros.hpp"

#include "hedley.h"

#include <memory>
#include <concepts>

// private macros end with a trailing underscore

#ifndef SDLRAII_THE_PREFIX
#  error "Please set SDLRAII_THE_PREFIX before including wrapgen_macros.hpp"
#endif

// would fn accept the arg types?
#define SDLRAII_REQUIRES_CALLABLE(sdl_name, ...)                               \
  requires std::invocable<decltype(sdl_name), __VA_ARGS__>

/**
 * To automate converting names to SDL_ or IMG_ etc names
 * PRECONDITION: SDLRAII_THE_PREFIX must be set
 */
#define SDLRAII_PUT_PREFIX(X) HEDLEY_CONCAT3(SDLRAII_THE_PREFIX, _, X)

/**
 * Generate a unique name. Used to prevent name collisions in macros that
 * introduce variables.
 * http://clhs.lisp.se/Body/f_gensym.htm
 */
#define SDLRAII_GENSYM(name)                                                   \
  HEDLEY_CONCAT3(gensym, __COUNTER__, name)

/**
 * DO NOT CALL DIRECTLY
 * Create a unique owning pointer named ~name~, holding object of type
 * ~THE_PREFIX ## name~, and deleted with ~destructor~ when given a unique
 * symbol.
 *
 * This macro needs a unique symbol because ~destructor~ could collide
 * with a non-unique name, breaking code in non-intuitive ways
 */
#define SDLRAII_DEFUNIQUE_(unique_name, ptr, name, destructor)                 \
  struct unique_name                                                           \
      : public std::unique_ptr<sdl::name, decltype(&destructor)> {             \
    unique_name(sdl::name* ptr = nullptr) noexcept                             \
        : std::unique_ptr<sdl::name, decltype(&destructor)>{ptr,               \
                                                            &destructor} {}    \
    unique_name(unique_name&&)      = default;                                 \
    unique_name(unique_name const&) = delete;                                  \
  };

/**
 * Create a unique owning pointer named ~name~, holding object of type
 * ~THE_PREFIX ## name~, and deleted with ~destructor~ when given a unique
 * symbol.
 */
#define SDLRAII_DEFUNIQUE(name, destructor)                                    \
  SDLRAII_DEFUNIQUE_(Unique##name, SDLRAII_GENSYM(ptr), name, destructor)

/**
 * Create an alias typename for ~SDL_name~ (or ~IMG_name~, etc)
 */
#define SDLRAII_WRAP_TYPE(name) using name = SDLRAII_PUT_PREFIX(name)

// constrained to allow overloading---accept types that the original SDL
// function can accept
#define SDLRAII_WRAP_MAKER_(Arg, arg, temp, raiitype, name, sdl_name)          \
  template<class... Arg>                                                       \
  SDLRAII_REQUIRES_CALLABLE(sdl_name, Arg...)                                  \
  inline sdl::MayError<raiitype> name(Arg... arg) noexcept {                   \
    auto* temp = sdl_name(arg...);                                             \
    SDLRAII_COLD_IF(temp == nullptr)                                           \
      return sdl::GetError();                                                  \
    return raiitype{temp};                                                     \
  }
/**
 * Wrap an SDL function into a function named ~name~ returning an object of type
 * ~raiitype~
 */
#define SDLRAII_WRAP_MAKER(raiitype, name)                                     \
  SDLRAII_WRAP_MAKER_(SDLRAII_GENSYM(Arg),                                     \
                      SDLRAII_GENSYM(arg),                                     \
                      SDLRAII_GENSYM(temp),                                    \
                      raiitype,                                                \
                      name,                                                    \
                      SDLRAII_PUT_PREFIX(name))

#define SDLRAII_WRAP_RENAME_FN_(Arg, arg, name, sdl_name, errorify)            \
  template<class... Arg>                                                       \
  SDLRAII_REQUIRES_CALLABLE(sdl_name, Arg...)                                  \
  inline auto name(Arg... arg) noexcept SDLRAII_DECL_RET(                      \
      errorify(sdl_name(arg...)));

#define SDLRAII_WRAP_RENAME_FN(new_name, sdl_name, errorify)                   \
  SDLRAII_WRAP_RENAME_FN_(SDLRAII_GENSYM(Arg),                                 \
                          SDLRAII_GENSYM(arg),                                 \
                          new_name,                                            \
                          sdl_name,                                            \
                          errorify)

/**
 * Create a variadic template that will call
 * ~<prefix>_name~
 */
#define SDLRAII_WRAP_FN(name, errorify)                                        \
  SDLRAII_WRAP_RENAME_FN(name, SDLRAII_PUT_PREFIX(name), errorify)

#define SDLRAII_WRAP_GETTER(name, input_t, return_t)                           \
  inline MayError<return_t> name(input_t* const self) {                        \
    return_t x;                                                                \
    SDLRAII_COLD_IF(SDLRAII_PUT_PREFIX(name)(self, &x) != 0)                   \
      return {sdl::GetError()};                                                \
    return x;                                                                  \
  }

#define SDLRAII_WRAP_RGB_GETTER_(name, sdl_name)                               \
  template<class T>                                                            \
  SDLRAII_REQUIRES_CALLABLE(sdl_name, T)                                       \
  inline MayError<rgb> name(T x) noexcept {                                    \
    Uint8 r, g, b;                                                             \
    SDLRAII_COLD_IF(sdl_name(x, &r, &g, &b) != 0)                              \
      return sdl::GetError();                                                  \
    return sdl::rgb{r, g, b};                                                  \
  }

#define SDLRAII_WRAP_RGB_GETTER(name)                                          \
  SDLRAII_WRAP_RGB_GETTER_(name, SDLRAII_PUT_PREFIX(name))

#define SDLRAII_WRAP_RGB_SETTER(name)                                          \
  inline auto name(auto x, sdl::rgb color)                                     \
      SDLRAII_BODY_EXP(name(x, color.r, color.g, color.b));

#endif // SDLRAII_WRAPGEN_MACROS_INCLUDE_GUARD
