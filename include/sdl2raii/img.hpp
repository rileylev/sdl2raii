#ifndef SDLRAII_IMG_INCLUDE_GUARD
#define SDLRAII_IMG_INCLUDE_GUARD

#include "mayerror.hpp"
#include "compat_macros.hpp"
#define SDLRAII_THE_PREFIX IMG
#include "wrapgen_macros.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace sdl { namespace img {

// init
struct init {
  enum flags { jpg = IMG_INIT_JPG, png = IMG_INIT_PNG, tif = IMG_INIT_TIF };
};

struct Quitter {
  ~Quitter() { sdl::img::Quit(); }
};

[[nodiscard]] MayError<Quitter> ScopedInit(init::flags flags = 0) noexcept {
  auto const result = sdl::Init(subsystems);
  SDLRAII_COLD_IF(!result.ok())
    return result.error();
  return Quitter{};
}

// automagic
SDLRAII_WRAP_FN(Init, nonzero_error);
SDLRAII_WRAP_RAIIFN(unique::Surface, Load);
SDLRAII_WRAP_RAIIFN(unique::Surface, Load_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadTyped_RW);
SDLRAII_WRAP_FN(Quit);

// specific
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadBMP_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadCUR_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadGIF_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadICO_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadJPG_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadLBM_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadPCX_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadPNG_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadPNM_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadTGA_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadTIF_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadXCF_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadXPM_RW);
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadXV_RW);

// array
SDLRAII_WRAP_RAIIFN(unique::Surface, ReadXPMFromArray);

// info
SDLRAII_WRAP_RAIIFN(unique::Surface, isBMP);
SDLRAII_WRAP_RAIIFN(unique::Surface, isCUR);
SDLRAII_WRAP_RAIIFN(unique::Surface, isGIF);
SDLRAII_WRAP_RAIIFN(unique::Surface, isICO);
SDLRAII_WRAP_RAIIFN(unique::Surface, isJPG);
SDLRAII_WRAP_RAIIFN(unique::Surface, isLBM);
SDLRAII_WRAP_RAIIFN(unique::Surface, isPCX);
SDLRAII_WRAP_RAIIFN(unique::Surface, isPNG);
SDLRAII_WRAP_RAIIFN(unique::Surface, isPNM);
SDLRAII_WRAP_RAIIFN(unique::Surface, isTIF);
SDLRAII_WRAP_RAIIFN(unique::Surface, isXCF);
SDLRAII_WRAP_RAIIFN(unique::Surface, isXPM);
SDLRAII_WRAP_RAIIFN(unique::Surface, isXV);

}} // namespace sdl::img

#undef SDLRAII_THE_PREFIX

#endif // SDLRAII_IMG_INCLUDE_GUARD
