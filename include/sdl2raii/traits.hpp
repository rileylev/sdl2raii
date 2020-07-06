#pragma once

#include <type_traits>

namespace sdl {
namespace traits {
template<class F>
struct function_traits;
template<class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {};

template<class R, class... Args>
struct function_traits<R(Args...)> {
  using return_type = R;
  using arg_types = std::tuple<Args...>;
};

template<class From, class To>
class all_convertible {
 private:
  static constexpr auto size = std::tuple_size_v<From>;
  static constexpr bool is_same_size = (size == std::tuple_size_v<To>);

  template<class>
  struct convertible_helper;

  template<auto i>
  static constexpr bool same_ith =
      std::is_convertible_v<std::tuple_element_t<i, From>,
                            std::tuple_element_t<i, To>>;

  template<auto... i>
  struct convertible_helper<std::index_sequence<i...>> {
    static constexpr bool value = (same_ith<i> && ...);
  };

 public:
  static constexpr bool value =
      is_same_size && convertible_helper<std::make_index_sequence<size>>::value;
};

template<class From, class To>
constexpr bool all_convertible_v = all_convertible<From, To>::value;

template<class Fn, class...Args>
constexpr bool would_Fn_take_Args_v =
  all_convertible_v<std::tuple<Args...>,
                    function_traits<Fn>::arg_types>;
}  // namespace traits
}  // namespace sdl
