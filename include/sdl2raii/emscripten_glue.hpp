#ifndef SDLRAII_EMSCRIPTEN_GLUE_INCLUDE_GUARD
#define SDLRAII_EMSCRIPTEN_GLUE_INCLUDE_GUARD

#include "compat_macros.hpp"

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif

/**
 *This namespace provides some utilities for managing the main loop. The goal
 * is to provide loop management tools that work the same from emscripten as
 * they do in a native program.
 */
namespace emscripten_glue {
template<class Thunk>
void call_thunk(void* detyped_thunk) {
  Thunk& t = *static_cast<Thunk*>(detyped_thunk);
  t();
}

#ifndef __EMSCRIPTEN__
static bool continue_main_loop = true;
#endif

#if true
/**
 * Start a main loop which calls ~iterate~ on every iteration
 */
template<class Thunk>
inline void main_loop(Thunk iterate) {
#  ifdef __EMSCRIPTEN__
  // emscripten_set_main_loop_arg uses a javascript exception to jump so
  // pointers to iterate won't dangle.
  emscripten_set_main_loop_arg(call_thunk<Thunk>, &iterate, -1, true);
#  else
  while(SDLRAII_LIKELY(continue_main_loop)) iterate();
#  endif
}

/**
 * Cancel the main loop
 */
inline void cancel_main_loop() {
#  ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#  else
  continue_main_loop = false;
#  endif
}
#endif
} // namespace emscripten_glue

#endif // SDLRAII_EMSCRIPTEN_GLUE_INCLUDE_GUARD
