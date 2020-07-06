#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "sdl2raii.hpp"

#define SDLRAII_THE_PREFIX IMG_
#include "sdl2raii_wrapgen_macros.hpp"

namespace sdl {
namespace img {
SDLRAII_WRAP_RAIIFN(unique::Surface, Load)
SDLRAII_WRAP_FN(Quit)
}  // namespace img
}  // namespace sdl

#undef SDLRAII_THE_PREFIX
#include "end_sdl2raii_wrapgen_macros.hpp"
