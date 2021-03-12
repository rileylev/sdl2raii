#ifndef SDLRAII_IMG_INCLUDE_GUARD
#define SDLRAII_IMG_INCLUDE_GUARD

#include "sdl2raii.hpp"
#define SDLRAII_THE_PREFIX IMG_
#include "wrapgen_macros.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace sdl { namespace img {
SDLRAII_WRAP_RAIIFN(unique::Surface, Load)
SDLRAII_WRAP_FN(Quit)
}} // namespace sdl::img

#undef SDLRAII_THE_PREFIX

#endif // SDLRAII_IMG_INCLUDE_GUARD
