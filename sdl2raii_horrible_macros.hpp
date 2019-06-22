#include <boost/preprocessor/cat.hpp>

// private macros end with a trailing underscore

#ifndef SDLRAII_THE_PREFIX
static_assert(false, "Please set SDLRAII_THE_PREFIX")
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
#define SDLRAII_GENSYM_(name)                                                  \
  BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(gensym, __COUNTER__), _), name)

/**
 * DO NOT CALL DIRECTLY
 * Create a unique owning pointer named ~name~, holding object of type
 * ~THE_PREFIX ## name~, and deleted with ~destructor~ when given a unique
 * symbol.
 *
 * This macro needs a unique symbol because destructor could collide
 * with a non-unique name breaking code in non-intuitive ways
 */
#define SDLRAII_DEFUNIQUE_WITH_SYM_(gensym, name, destructor)                  \
  struct name : public std::unique_ptr<SDLRAII_PUT_PREFIX(name),               \
                                       decltype(&destructor)> {                \
    name(SDLRAII_PUT_PREFIX(name) * gensym)                                    \
        : std::unique_ptr<SDLRAII_PUT_PREFIX(name), decltype(&destructor)>{    \
            gensym,                                                            \
            &destructor} {}                                                    \
    name() : name(nullptr){};                                                  \
  };

/**
 * Create a unique owning pointer named ~name~, holding object of type
 * ~THE_PREFIX ## name~, and deleted with ~destructor~ when given a unique
 * symbol.
 */
#define SDLRAII_DEFUNIQUE(name, destructor)                                    \
  SDLRAII_DEFUNIQUE_WITH_SYM_(SDLRAII_GENSYM_(name), name, destructor)

/**
 * Create an alias name for ~SDL_name~ (or ~IMG_name~, etc)
 */
#define SDLRAII_WRAP_TYPE(name) using name = SDLRAII_PUT_PREFIX(name)

/**
 * Wrap an SDL function into a function named ~name~ returning an object of type
 * ~raiitype~
 */
#define SDLRAII_WRAP_RAIIFN(raiitype, name)                                    \
  template<class... T>                                                         \
  inline auto name(T&&... arg) noexcept {                                      \
    auto* thing = SDLRAII_PUT_PREFIX(name)(std::forward<T>(arg)...);           \
    return raiitype{thing};                                                    \
  }

/**
 * Wrap a create function called ~fname~ to a version that errors if nullptr
 * would be returned
 */
#define SDLRAII_WRAP_CREATE_THROW_IF_NULL(fnname)                              \
  template<class... T>                                                         \
  inline auto fnname(T&&... arg) {                                             \
    auto owner = sdl::fnname(std::forward<T>(arg)...);                         \
    if(owner == nullptr) {                                                     \
      owner.release();                                                         \
      throw CreateFailed(SDL_GetError());                                      \
    }                                                                          \
    return owner;                                                              \
  }

/**
 * Create a variadic template that will call ~<prefix>_name~ by forwarding
 * whatever you pass it.
 *
 * WARNING: will match any arguments. If you want to overload, use carefully
 */
#define SDLRAII_WRAP_FN(name)                                                  \
  template<class... T>                                                         \
  inline auto name(T&&... arg) noexcept {                                      \
    return SDLRAII_PUT_PREFIX(name)(std::forward<T>(arg)...);                  \
  }
