#pragma once

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#else
#  include <chrono>
#endif

namespace emscripten_glue {
template<class Thunk>
void call_thunk(void* detyped_thunk) {
  Thunk& t = *static_cast<Thunk*>(detyped_thunk);
  t();
}

#ifndef __EMSCRIPTEN__
  static bool continue_main_loop = true;
#endif

template<class Thunk>
inline void main_loop(Thunk iterate) {
  // uses an exception to jump so pointers to iterate won't dangle. Still, this is awful.
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(call_thunk<Thunk>, &iterate, -1, true);
#else
  while(continue_main_loop){
    iterate();
  }
#endif
}

inline void cancel_main_loop() {
#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#else
  continue_main_loop=false;
#endif
}
}  // namespace emscripten_glue
