#include <variant>
#include <string>

namespace sdl {

struct Error {
  char const* const message;
  Error() = default;
  Error(char const* const message) : message{message} {}
  static auto getError() { return Error{SDL_GetError()}; }
};

/**
 * Strongly-typed, possibly ignorable,  error codes.  For example, this is a
 * closer idiomatic fit to SDL's return codes. Some of these errors are too
 * small/routine to warrant a whole try/catch block. Some games don't/can't use
 * exceptions.
 */
template<class Success>
class MayError {
  static constexpr int good_idx = 0;
  static constexpr int bad_idx = 1;
  std::variant<Success, Error> it;

  static constexpr bool is_move_nothrow =
      std::is_nothrow_move_constructible_v<Success>;
  static constexpr bool can_copy = std::is_copy_constructible_v<Success>;

 public:
  MayError() = default;
  MayError(Success success) noexcept(is_move_nothrow)
      : it{std::move(success)} {}
  MayError(Error error) noexcept(is_move_nothrow) : it{std::move(error)} {}

  bool ok() const { return it.index() == good_idx; }

#define GETTERS(constref, maybemove)                                           \
  inline auto constref success() constref {                                    \
    return maybemove(std::get<good_idx>(it));                                  \
  }                                                                            \
  inline auto constref error() constref {                                      \
    return maybemove(std::get<bad_idx>(it));                                   \
  }

  GETTERS(&, )
  GETTERS(const&, )
  GETTERS(&&, std::move)
  GETTERS(const&&, std::move)  // https://youtu.be/J4A2B9eexiw?t=1479
#undef GETTERS

  /*implicit*/ operator Success() & { return success(); }
  /*implicit*/ operator Success() && { return std::move(success()); }

#define GETORELSE(constref, maybemove)                                         \
  template<class F>                                                            \
  inline Success get_or_else(F on_error) constref {                            \
    if(it.index() == good_idx)                                                 \
      return maybemove(success());                                             \
    else                                                                       \
      return on_error(error());                                                \
  }
  GETORELSE(&, )
  GETORELSE(const&, )
  GETORELSE(&&, std::move)
  GETORELSE(const&&, std::move)
#undef GETORELSE

  inline decltype(auto) get_or_throw() {
    return get_or_else([](auto error) -> Success { throw error; });
  }
};
}  // namespace sdl
