#include <boost/preprocessor/cat.hpp>
#include "traits.hpp"

// private macros end with a trailing underscore

#ifndef SDLRAII_THE_PREFIX
static_assert(
    false,
    "Please set SDLRAII_THE_PREFIX before including sdl2raii_macros.hpp")
#endif

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

// constrained to allow overloading---accept types that the original SDL function can accept
#define SDLRAII_WRAP_RAIIFN_(Arg, arg, temp, raiitype, name, sdl_name)         \
  template<class... Arg,                                                       \
           class = std::enable_if_t<sdl::traits::all_convertible_v<            \
               std::tuple<Arg...>,                                             \
               sdl::traits::function_traits<decltype(sdl_name)>::arg_types>>>  \
  sdl::MayError<raiitype> name(Arg... arg) {                                   \
    auto* temp = sdl_name(arg...);                                             \
    if(temp != nullptr)                                                        \
      return raiitype{temp};                                                   \
    else                                                                       \
      return sdl::Error::getError();                                           \
  }
/**
 * Wrap an SDL function into a function named ~name~ returning an object of type
 * ~raiitype~
 */
#define SDLRAII_WRAP_RAIIFN(raiitype, name)                                    \
  SDLRAII_WRAP_RAIIFN_(SDLRAII_GENSYM(Arg),                                    \
                       SDLRAII_GENSYM(arg),                                    \
                       SDLRAII_GENSYM(temp),                                   \
                       raiitype,                                               \
                       name,                                                   \
                       SDLRAII_PUT_PREFIX(name))
// example expansion:
// template<class... Arg,
//          class =
//          std::enable_if_t<sdl::traits::all_convertible_v<
//              std::tuple<Arg...>,
//              sdl::traits::function_traits<
//                  decltype(SDL_CreateTextureFromSurface)>::arg_types>>>
// sdl::MayError<unique::Texture>
// CreateTextureFromSurface(Arg... arg) {
//   if(auto* tex = SDL_CreateTextureFromSurface(arg...))
//     return sdl::unique::Texture{tex};
//   else
//     return sdl::Error::getError();
// }

#define SDLRAII_WRAP_FN_(Arg, arg, name, sdl_name)                             \
  template<class... Arg,                                                       \
           class = std::enable_if_t<sdl::traits::all_convertible_v<            \
               std::tuple<Arg...>,                                             \
               sdl::traits::function_traits<decltype(sdl_name)>::arg_types>>>  \
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
    if(SDLRAII_PUT_PREFIX(name)(self, &x) != 0)                                \
      return Error::getError();                                                \
    return x;                                                                  \
  }
