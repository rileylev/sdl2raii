#include <boost/preprocessor/cat.hpp>

#include "compat_macros.hpp"
#include "traits.hpp"

// private macros end with a trailing underscore

#ifndef SDLRAII_THE_PREFIX
#  error "Please set SDLRAII_THE_PREFIX before including wrapgen_macros.hpp"
#endif

// would fn accept the arg types?
#define SDLRAII_ARGS_OK(fn, ...)                                               \
  sdl::traits::would_Fn_take_Args_v<decltype(fn), __VA_ARGS__>

/**
 * To automate converting names to SDL_ or IMG_ etc names
 * PRECONDITION: SDLRAII_THE_PREFIX must be set
 */
#define SDLRAII_PUT_PREFIX(X) BOOST_PP_CAT(SDLRAII_THE_PREFIX, X)

/**
 * Generate a unique name. Used to prevent name collisions in macros that
 * introduce variables.
 * http://clhs.lisp.se/Body/f_gensym.htm
 */
#define SDLRAII_GENSYM(name)                                                   \
  BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(gensym, __COUNTER__), _), name)

/**
 * DO NOT CALL DIRECTLY
 * Create a unique owning pointer named ~name~, holding object of type
 * ~THE_PREFIX ## name~, and deleted with ~destructor~ when given a unique
 * symbol.
 *
 * This macro needs a unique symbol because ~destructor~ could collide
 * with a non-unique name, breaking code in non-intuitive ways
 */
#define SDLRAII_DEFUNIQUE_(gensym, name, destructor)                           \
  struct name : public std::unique_ptr<SDLRAII_PUT_PREFIX(name),               \
                                       decltype(&destructor)> {                \
    name(SDLRAII_PUT_PREFIX(name) * gensym = nullptr)                          \
        : std::unique_ptr<SDLRAII_PUT_PREFIX(name), decltype(&destructor)>{    \
            gensym,                                                            \
            &destructor} {}                                                    \
    name(name&&) = default;                                                    \
    name(name const&) = delete;                                                \
  };

/**
 * Create a unique owning pointer named ~name~, holding object of type
 * ~THE_PREFIX ## name~, and deleted with ~destructor~ when given a unique
 * symbol.
 */
#define SDLRAII_DEFUNIQUE(name, destructor)                                    \
  SDLRAII_DEFUNIQUE_(SDLRAII_GENSYM(name), name, destructor)

/**
 * Create an alias typename for ~SDL_name~ (or ~IMG_name~, etc)
 */
#define SDLRAII_WRAP_TYPE(name) using name = SDLRAII_PUT_PREFIX(name)

// constrained to allow overloading---accept types that the original SDL
// function can accept
#define SDLRAII_WRAP_MAKER_(Arg, arg, temp, raiitype, name, sdl_name)          \
  template<class... Arg,                                                       \
           class = std::enable_if_t<SDLRAII_ARGS_OK(sdl_name, Arg...)>>        \
  inline sdl::MayError<raiitype> name(Arg... arg) {                            \
    auto* temp = sdl_name(arg...);                                             \
    if(SDLRAII_UNLIKELY(temp == nullptr)) return sdl::Error::getError();       \
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

#define SDLRAII_WRAP_FN_(Arg, arg, name, sdl_name)                             \
  template<class... Arg,                                                       \
           class = std::enable_if_t<SDLRAII_ARGS_OK(sdl_name, Arg...)>>        \
  inline auto name(Arg... arg) noexcept {                                      \
    return sdl_name(arg...);                                                   \
  }
/**
 * Create a variadic template that will call
 * ~<prefix>_name~
 */
#define SDLRAII_WRAP_FN(name)                                                  \
  SDLRAII_WRAP_FN_(SDLRAII_GENSYM(Arg),                                        \
                   SDLRAII_GENSYM(arg),                                        \
                   name,                                                       \
                   SDLRAII_PUT_PREFIX(name))

#define SDLRAII_WRAP_GETTER(name, input_t, return_t)                           \
  inline MayError<return_t> name(input_t* const self) {                        \
    return_t x;                                                                \
    if(SDLRAII_UNLIKELY(SDLRAII_PUT_PREFIX(name)(self, &x) != 0))              \
      return Error::getError();                                                \
    return x;                                                                  \
  }
