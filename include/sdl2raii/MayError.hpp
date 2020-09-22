#ifndef SDLRAII_MAYERROR_INCLUDE_GUARD
#define SDLRAII_MAYERROR_INCLUDE_GUARD

#include "compat_macros.hpp"

#include <SDL2/SDL.h>

#include <variant>
#include <string>

namespace sdl {

#ifndef SDLRAII_MAYERROR_ASSERT
#  define SDLRAII_MAYERROR_ASSERT(...) ((void)0)
#endif

struct Error {
  char const* message = nullptr;
  Error() = default;
  explicit Error(char const* const message) : message{message} {}
};

inline Error getError() noexcept { return Error{SDL_GetError()}; }

#if true
/**
 * Strongly-typed, possibly ignorable,  error codes.  For example, this is a
 * closer idiomatic fit to SDL's return codes. Some of these errors are too
 * small/routine to warrant a whole try/catch block. Some games don't/can't use
 * exceptions.
 */
template<class Success>
class MayError {
 public:
  MayError() = default;
  explicit(false) MayError(Success success) noexcept(is_move_nothrow)
      : it{std::move(success)} {}
  explicit(false) MayError(Error error) noexcept(is_move_nothrow)
      : it{std::move(error)} {}

  bool ok() const { return it.index() == good_idx; }

#  define GETTERS(constref, move_)                                             \
    auto constref success() constref {                                         \
      SDLRAII_MAYERROR_ASSERT(ok());                                           \
      return move_(std::get<good_idx>(it));                                    \
    }                                                                          \
    auto constref error() constref {                                           \
      SDLRAII_MAYERROR_ASSERT(!ok());                                          \
      return move_(std::get<bad_idx>(it));                                     \
    }                                                                          \
    decltype(auto) get() constref { return move_(*this).success(); }

  GETTERS(&, )
  GETTERS(const&, )
  GETTERS(&&, std::move)
  GETTERS(const&&, std::move) // https://youtu.be/J4A2B9eexiw?t=1479
#  undef GETTERS

 private:
  static constexpr int good_idx = 0;
  static constexpr int bad_idx = 1;
  std::variant<Success, Error> it;

  static constexpr bool is_move_nothrow =
      std::is_nothrow_move_constructible_v<Success>;
  static constexpr bool can_copy = std::is_copy_constructible_v<Success>;
};

template<>
class MayError<void> {
 public:
  MayError() = default;
  explicit(false) MayError(Error error) : error_{error} {}
  bool ok() const { return error_.message == nullptr; }

  using Success = void;
  Error error() noexcept { return error_; }
  void success() { return; }
  void get() { return; }

 private:
  Error error_;
};

#endif
} // namespace sdl

#endif // SDLRAII_MAYERROR_INCLUDE_GUARD
