#ifndef SDLRAII_MAYERROR_INCLUDE_GUARD
#define SDLRAII_MAYERROR_INCLUDE_GUARD

#include "compat_macros.hpp"

#include <SDL2/SDL.h>

#include <variant>
#include <string>

namespace sdl {

#ifndef SDLRAII_MAYERROR_ASSERT
#  define SDLRAII_MAYERROR_ASSERT(...) SDL_assert(__VA_ARGS__)
#endif

struct Error {
  char const* message = nullptr;
  Error()             = default;
  explicit Error(char const* const message) : message{message} {}
};

inline Error GetError() noexcept { return Error{SDL_GetError()}; }

#if true
/**
 * Strongly-typed, possibly ignorable error codes.  For example, this is a
 * closer idiomatic fit to SDL's return codes. Some of these errors are too
 * small/routine to warrant a whole try/catch block. Some games don't/can't use
 * exceptions.
 */
template<class Success>
class MayError {
 private:
  static constexpr int good_idx = 0;
  static constexpr int bad_idx  = 1;
  std::variant<Success, Error> data_;

  static constexpr bool is_move_nothrow =
      std::is_nothrow_move_constructible_v<Success>;

 public:
  MayError() = default;
  explicit(false) MayError(Success success) noexcept(is_move_nothrow)
      : data_{std::move(success)} {}
  explicit(false) MayError(Error error) noexcept(true)
      : data_{std::move(error)} {}

  bool ok() const { return data_.index() == good_idx; }

#  define SDLRAII_GETTERS(constref, move_)                                     \
    auto constref success() constref {                                         \
      SDLRAII_MAYERROR_ASSERT(ok());                                           \
      return move_(std::get<good_idx>(data_));                                 \
    }                                                                          \
    auto constref error() constref {                                           \
      SDLRAII_MAYERROR_ASSERT(!ok());                                          \
      return move_(std::get<bad_idx>(data_));                                  \
    }                                                                          \
    decltype(auto) get() constref { return move_(*this).success(); }

  SDLRAII_GETTERS(&, )
  SDLRAII_GETTERS(const&, )
  SDLRAII_GETTERS(&&, std::move)
  SDLRAII_GETTERS(const&&, std::move) // https://youtu.be/J4A2B9eexiw?t=1479
#  undef SDLRAII_GETTERS
};

template<>
class MayError<void> {
 public:
  MayError() = default;
  explicit(false) MayError(Error error) : error_{error} {}

  bool ok() const { return error_.message == nullptr; }

  using Success = void;
  Error error() const noexcept { return error_; }
  void success() const noexcept { return; }
  void get() const noexcept { return; }

 private:
  Error error_;
};

template<class T>
inline MayError<T> nonzero_error(T x) {
  SDLRAII_COLD_IF(x != 0)
    return sdl::GetError();
  return x;
}
#endif
} // namespace sdl

#define SDLRAII_BAIL_ERROR(x) do {SDLRAII_COLD_IF(!x.ok()) return x.error(); } while(false)


#endif // SDLRAII_MAYERROR_INCLUDE_GUARD
