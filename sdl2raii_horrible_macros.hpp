#include <boost/preprocessor/cat.hpp>

// private macros end with a trailing underscore

// to automate converting names to SDL_ or IMG_ etc names
#define SDLRAII_PUT_PREFIX(X) BOOST_PP_CAT(SDLRAII_THE_PREFIX, X)

// generate a unique name
#define SDLRAII_GENSYM_(name)                                                  \
  BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(gensym, __COUNTER__), _), name)

// needs a unique name
#define SDLRAII_DEFUNIQUE_WITH_SYM_(gensym, name, destructor)                  \
  struct name : public std::unique_ptr<SDLRAII_PUT_PREFIX(name),               \
                                       decltype(&destructor)> {                \
    name(SDLRAII_PUT_PREFIX(name) * gensym)                                    \
        : std::unique_ptr<SDLRAII_PUT_PREFIX(name), decltype(&destructor)>{    \
            gensym,                                                            \
            &destructor} {}                                                    \
    name() : name(nullptr){};                                                  \
  };

// defines a unique pointer wrapper that owns a pointer to an SDL resource type
#define SDLRAII_DEFUNIQUE(name, destructor)                                    \
  SDLRAII_DEFUNIQUE_WITH_SYM_(SDLRAII_GENSYM_(name), name, destructor)

// create an alias name for SDL_name (or IMG_name, etc)
#define SDLRAII_WRAP_TYPE(name) using name = SDLRAII_PUT_PREFIX(name)

// wrap an SDL function into a function returning an owning unique ptr
#define SDLRAII_WRAP_RAIIFN(raiitype, name)                                    \
  template<class... T>                                                         \
  inline auto name(T&&... arg) noexcept {                                      \
    auto* thing = SDLRAII_PUT_PREFIX(name)(std::forward<T>(arg)...);           \
    return raiitype{thing};                                                    \
  }

// throw if creation fails
#define SDLRAII_WRAP_CREATE_THROW_IF_NULL(fnname)                              \
  template<class... T>                                                         \
  inline auto fnname(T&&... arg) {                                             \
    auto owner = sdl::fnname(std::forward<T>(arg)...);                         \
    if(owner == nullptr) {                                                     \
      owner.release();                                                         \
      throw_or_die(CreateFailed(SDL_GetError()));                              \
    }                                                                          \
    return owner;                                                              \
  }

// will call SDL_name (SDL_THE_PREFIX_name) with whatever arguments you pass in
// WARNING: will match any arguments. If you want to overload, do not use this
#define SDLRAII_WRAP_FN(name)                                                  \
  template<class... T>                                                         \
  inline auto name(T&&... arg) noexcept {                                      \
    return SDLRAII_PUT_PREFIX(name)(std::forward<T>(arg)...);                  \
  }
