#pragma once

#include <memory>
#include <tuple>
#include <optional>

#include <SDL2/SDL.h>
#include <boost/preprocessor/cat.hpp>

#include "compat_macros.hpp"

#define SDLRAII_THE_PREFIX SDL_
#include "wrapgen_macros.hpp"
#include "MayError.hpp"

// SDL headers define this as a macro which doesn't work with the TMP
#undef SDL_LoadBMP
inline auto SDL_LoadBMP(char const* file) {
  return SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1);
}

namespace sdl {

/**
 * Unique_ptr owning types. They automatically call the correct SDL_... deleter
 * function.
 */
namespace unique {
SDLRAII_DEFUNIQUE(Window, SDL_DestroyWindow);
SDLRAII_DEFUNIQUE(Renderer, SDL_DestroyRenderer);
SDLRAII_DEFUNIQUE(Surface, SDL_FreeSurface);
SDLRAII_DEFUNIQUE(Texture, SDL_DestroyTexture);
} // namespace unique

SDLRAII_WRAP_TYPE(Window);
SDLRAII_WRAP_TYPE(Renderer);
SDLRAII_WRAP_TYPE(Surface);
SDLRAII_WRAP_TYPE(Texture);
SDLRAII_WRAP_TYPE(Rect);
SDLRAII_WRAP_TYPE(Point);
SDLRAII_WRAP_TYPE(RendererFlip);

/**
 * SDL_FLIP_... as named constants
 */
namespace flip {
enum flags : Uint32 {
  none = SDL_FLIP_NONE,
  horizontal = SDL_FLIP_HORIZONTAL,
  vertical = SDL_FLIP_VERTICAL
};
} // namespace flip

SDLRAII_WRAP_RAIIFN(unique::Window, CreateWindow)
SDLRAII_WRAP_RAIIFN(unique::Renderer, CreateRenderer)
SDLRAII_WRAP_RAIIFN(unique::Texture, CreateTextureFromSurface)
SDLRAII_WRAP_RAIIFN(unique::Surface, LoadBMP)

template<class T>
inline auto CreateWindowFrom(T* data) {
  return unique::Window{SDL_CreateWindowFrom(static_cast<void*>(data))};
}

/**
 * Creates a texture from a ~unique::Surface~. Deletes the ~unique::Surface~
 * after.
 */
inline sdl::MayError<unique::Texture>
    CreateTextureFromSurface(Renderer* const renderer,
                             unique::Surface surface) {
  return CreateTextureFromSurface(renderer, surface.get());
}

namespace window {
constexpr auto pos_undefined = SDL_WINDOWPOS_UNDEFINED;
enum flags : Uint32 {
  fullscreen = SDL_WINDOW_FULLSCREEN,
  fullscreen_desktop = SDL_WINDOW_FULLSCREEN_DESKTOP,
  opengl = SDL_WINDOW_OPENGL,
  shown = SDL_WINDOW_SHOWN,
  hidden = SDL_WINDOW_HIDDEN,
  borderless = SDL_WINDOW_BORDERLESS,
  resizable = SDL_WINDOW_RESIZABLE,
  minimized = SDL_WINDOW_MINIMIZED,
  maximized = SDL_WINDOW_MAXIMIZED,
  input_grabbed = SDL_WINDOW_INPUT_GRABBED,
  input_focus = SDL_WINDOW_INPUT_FOCUS,
  mouse_focus = SDL_WINDOW_MOUSE_FOCUS,
  foreign = SDL_WINDOW_FOREIGN,
  allow_highdpi = SDL_WINDOW_ALLOW_HIGHDPI,
  mouse_capture = SDL_WINDOW_MOUSE_CAPTURE,
  always_on_top = SDL_WINDOW_ALWAYS_ON_TOP,
  skip_taskbar = SDL_WINDOW_SKIP_TASKBAR,
  utility = SDL_WINDOW_UTILITY,
  tooltip = SDL_WINDOW_TOOLTIP,
  popup_menu = SDL_WINDOW_POPUP_MENU
};
} // namespace window

inline MayError<std::tuple<unique::Window, unique::Renderer>>
    CreateWindowAndRenderer(int const width,
                            int const height,
                            window::flags const flags) noexcept {
  Window* win;
  Renderer* ren;
  if (SDLRAII_UNLIKELY(
          SDL_CreateWindowAndRenderer(width, height, flags, &win, &ren)
          == nullptr))
    return Error::getError();
  return std::make_tuple(unique::Window{win}, unique::Renderer{ren});
}

SDLRAII_WRAP_FN(Init);
SDLRAII_WRAP_FN(Quit);
SDLRAII_WRAP_FN(RenderClear);

inline auto RenderCopy(Renderer* const renderer,
                       Texture* const texture,
                       Rect const* srcrect,
                       Rect const* dstrect) noexcept {
  return SDL_RenderCopy(renderer, texture, srcrect, dstrect);
}

namespace impl {
/**
 * C libraries use pointers to indicate a value can be null. Here we
 * translate an optional into a pointer either to its contents or to nullptr
 * if empty.
 */
template<class T>
inline auto optional_to_ptr(std::optional<T const> const& x) {
  return x ? &*x : nullptr;
}
} // namespace impl

inline auto RenderCopy(Renderer* const renderer,
                       Texture* const texture,
                       std::optional<Rect const> const srcrect,
                       std::optional<Rect const> const dstrect) noexcept {
  return RenderCopy(renderer,
                    texture,
                    impl::optional_to_ptr(srcrect),
                    impl::optional_to_ptr(dstrect));
}

SDLRAII_WRAP_FN(RenderPresent);
SDLRAII_WRAP_FN(SetRenderDrawColor);
SDLRAII_WRAP_FN(RenderDrawLine);
SDLRAII_WRAP_FN(RenderDrawLines);
SDLRAII_WRAP_FN(RenderDrawPoint);
SDLRAII_WRAP_FN(RenderDrawPoints);
SDLRAII_WRAP_FN(RenderDrawRects);
SDLRAII_WRAP_FN(RenderFillRects);

inline auto RenderDrawRect(Renderer* renderer, Rect const rect) {
  return SDL_RenderDrawRect(renderer, &rect);
}

inline auto RenderFillRect(Renderer* renderer, Rect const rect) {
  return SDL_RenderFillRect(renderer, &rect);
}

struct rgba {
  Uint8 r, g, b, a;
};

inline auto SetRenderDrawColor(Renderer* const renderer, rgba const color) {
  auto [r, g, b, a] = color;
  SetRenderDrawColor(renderer, r, g, b, a);
}

/**
 * Gets the current draw color. Returns an ~rgba~ object instead of using
 * out parameters.
 */
inline auto GetRenderDrawColor(Renderer* const renderer) {
  Uint8 r, g, b, a;
  SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
  return rgba{r, g, b, a};
}

template<class N>
struct degrees {
  N number;
  degrees() = default;
  constexpr explicit degrees(N number) noexcept : number{number} {}
};

inline auto RenderCopyEx(Renderer* const renderer,
                         Texture* const texture,
                         Rect const* const src,
                         Rect const* const dst,
                         degrees<double const> const angle,
                         Point const* const center,
                         RendererFlip const flip) {
  return SDL_RenderCopyEx(renderer,
                          texture,
                          src,
                          dst,
                          angle.number,
                          center,
                          flip);
}

inline auto RenderCopyEx(Renderer* const renderer,
                         Texture* const texture,
                         std::optional<Rect const> const src,
                         std::optional<Rect const> const dst,
                         degrees<double const> const angle,
                         std::optional<Point const> const center,
                         RendererFlip const flip) {
  return RenderCopyEx(renderer,
                      texture,
                      impl::optional_to_ptr(src),
                      impl::optional_to_ptr(dst),
                      angle,
                      impl::optional_to_ptr(center),
                      flip);
}

inline auto IntersectRect(Rect const* const A, Rect const* const B) {
  Rect result;
  if (SDL_IntersectRect(A, B, &result))
    return std::make_optional(std::move(result));
  else
    return std::nullopt;
}
inline auto IntersectRect(Rect const A, Rect const B) {
  return IntersectRect(&A, &B);
}

SDLRAII_WRAP_FN(HasIntersection);
inline auto HasIntersection(Rect const A, Rect const B) {
  return HasIntersection(&A, &B);
}

namespace init {
enum flags : Uint32 {
  timer = SDL_INIT_TIMER,
  audio = SDL_INIT_AUDIO,
  video = SDL_INIT_VIDEO,
  joystick = SDL_INIT_JOYSTICK,
  haptic = SDL_INIT_HAPTIC,
  gamecontroller = SDL_INIT_GAMECONTROLLER,
  events = SDL_INIT_EVENTS,
  everything = SDL_INIT_EVERYTHING,
};
}
namespace renderer {
enum flags : Uint32 {
  software = SDL_RENDERER_SOFTWARE,
  accelerated = SDL_RENDERER_ACCELERATED,
  presentvsync = SDL_RENDERER_PRESENTVSYNC,
  targettexture = SDL_RENDERER_TARGETTEXTURE
};
}

SDLRAII_WRAP_TYPE(Event);
inline bool HasNextEvent() noexcept { return SDL_PollEvent(nullptr); }
inline auto NextEvent() noexcept {
  Event e;
  return SDL_PollEvent(&e) ? std::make_optional(e) : std::nullopt;
}

namespace BlendMode {
using type = SDL_BlendMode;
auto const none = SDL_BLENDMODE_NONE;
auto const blend = SDL_BLENDMODE_BLEND;
auto const add = SDL_BLENDMODE_ADD;
auto const mod = SDL_BLENDMODE_MOD;
} // namespace BlendMode

SDLRAII_WRAP_FN(SetTextureBlendMode);
SDLRAII_WRAP_FN(SetSurfaceBlendMode);
SDLRAII_WRAP_FN(SetRenderDrawBlendMode);
SDLRAII_WRAP_FN(SetTextureAlphaMod)
SDLRAII_WRAP_FN(SetTextureColorMod)
SDLRAII_WRAP_FN(SetSurfaceAlphaMod)
SDLRAII_WRAP_FN(SetSurfaceColorMod)

SDLRAII_WRAP_GETTER(GetRenderDrawBlendMode, Renderer, BlendMode::type);
SDLRAII_WRAP_GETTER(GetTextureBlendMode, Texture, BlendMode::type);
SDLRAII_WRAP_GETTER(GetSurfaceBlendMode, Surface, BlendMode::type);
SDLRAII_WRAP_GETTER(GetTextureAlphaMod, Texture, Uint8);
SDLRAII_WRAP_GETTER(GetSurfaceAlphaMod, Surface, Uint8);

struct rgb {
  Uint8 r, g, b;
};

inline MayError<rgb> GetSurfaceColorMod(Surface* const surface) {
  Uint8 r, g, b;
  if (SDLRAII_UNLIKELY(SDL_GetSurfaceColorMod(surface, &r, &g, &b)))
    return Error::getError();
  return rgb{r, g, b};
}

inline MayError<rgb> GetTextureColorMod(Texture* const texture) {
  Uint8 r, g, b;
  if (SDLRAII_UNLIKELY(SDL_GetTextureColorMod(texture, &r, &g, &b)))
    return Error::getError();
  return rgb{r, g, b};
}

SDLRAII_WRAP_FN(SetWindowIcon)
} // namespace sdl
#undef SDLRAII_THE_PREFIX
#include "end_wrapgen_macros.hpp"
