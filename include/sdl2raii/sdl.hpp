#ifndef SDLRAII_SDL_INCLUDE_GUARD
#define SDLRAII_SDL_INCLUDE_GUARD

#include "compat_macros.hpp"

#define SDLRAII_THE_PREFIX SDL_
#include "wrapgen_macros.hpp"
#include "MayError.hpp"

#include <SDL2/SDL.h>
#include <boost/preprocessor/cat.hpp>

#include <memory>
#include <tuple>
#include <optional>

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

SDLRAII_WRAP_TYPE(Window);
SDLRAII_WRAP_TYPE(Renderer);
SDLRAII_WRAP_TYPE(Surface);
SDLRAII_WRAP_TYPE(Texture);
SDLRAII_WRAP_TYPE(Rect);
SDLRAII_WRAP_TYPE(Point);
SDLRAII_WRAP_TYPE(RendererFlip);

// https://en.cppreference.com/w/cpp/memory/unique_ptr/%7Eunique_ptr
// If get() == nullptr there are no effects. Otherwise, the owned object is
// destroyed via get_deleter()(get()).
// Requires that get_deleter()(get()) does not throw exceptions.
SDLRAII_DEFUNIQUE(Window, SDL_DestroyWindow);
SDLRAII_DEFUNIQUE(Renderer, SDL_DestroyRenderer);
SDLRAII_DEFUNIQUE(Surface, SDL_FreeSurface);
SDLRAII_DEFUNIQUE(Texture, SDL_DestroyTexture);

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

SDLRAII_WRAP_MAKER(UniqueWindow, CreateWindow)
SDLRAII_WRAP_MAKER(UniqueRenderer, CreateRenderer)
SDLRAII_WRAP_MAKER(UniqueTexture, CreateTextureFromSurface)
SDLRAII_WRAP_MAKER(UniqueSurface, LoadBMP)

template<class T>
inline auto CreateWindowFrom(T* data) SDLRAII_BODY_EXP(
    UniqueWindow(SDL_CreateWindowFrom(static_cast<void*>(data))));

/**
 * Creates a texture from a ~unique::Surface~. Deletes the ~unique::Surface~
 * after.
 */
inline sdl::MayError<UniqueTexture>
    CreateTextureFromSurface(Renderer* const renderer, UniqueSurface surface) {
  return CreateTextureFromSurface(renderer, surface.get());
}

namespace window {
[[maybe_unused]] constexpr auto pos_undefined = SDL_WINDOWPOS_UNDEFINED;
[[maybe_unused]] constexpr auto pos_centered = SDL_WINDOWPOS_CENTERED;
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

inline auto
    CreateWindow(const char* title, int w, int h, window::flags flags = {})
        SDLRAII_BODY_EXP(CreateWindow(title,
                                      window::pos_undefined,
                                      window::pos_undefined,
                                      w,
                                      h,
                                      flags));

inline MayError<std::tuple<UniqueWindow, UniqueRenderer>>
    CreateWindowAndRenderer(int const width,
                            int const height,
                            window::flags const flags) noexcept {
  Window* win;
  Renderer* ren;
  SDLRAII_COLD_IF(SDL_CreateWindowAndRenderer(width, height, flags, &win, &ren)
                  < 0)
    return sdl::getError();
  return std::tuple(UniqueWindow{win}, UniqueRenderer{ren});
}

template<class T>
inline MayError<T> nonzero_error(T x) {
  SDLRAII_COLD_IF(x != 0)
    return sdl::getError();
  else return x;
}

SDLRAII_WRAP_FN(Init, nonzero_error);
SDLRAII_WRAP_FN(Quit, );

struct Quitter {
  ~Quitter() { sdl::Quit(); }
};

namespace init {
using flags = Uint32;
[[maybe_unused]] constexpr flags timer = SDL_INIT_TIMER;
[[maybe_unused]] constexpr flags audio = SDL_INIT_AUDIO;
[[maybe_unused]] constexpr flags video = SDL_INIT_VIDEO;
[[maybe_unused]] constexpr flags joystick = SDL_INIT_JOYSTICK;
[[maybe_unused]] constexpr flags haptic = SDL_INIT_HAPTIC;
[[maybe_unused]] constexpr flags gamecontroller = SDL_INIT_GAMECONTROLLER;
[[maybe_unused]] constexpr flags events = SDL_INIT_EVENTS;
[[maybe_unused]] constexpr flags everything = SDL_INIT_EVERYTHING;
} // namespace init

[[nodiscard]] MayError<Quitter>
    ScopedInit(init::flags subsystems = {}) noexcept {
  auto const result = sdl::Init(subsystems);
  SDLRAII_COLD_IF(!result.ok())
    return result.error();
  return Quitter{};
}

SDLRAII_WRAP_FN(RenderClear, nonzero_error);
SDLRAII_WRAP_FN(RenderCopy, nonzero_error);

namespace impl {
/**
 * C uses pointers where an optional would be appropriate in C++.
 * This function bridges the gap: return the address if the optional has value,
 * return nullptr otherwise.
 */
// will this dangle?
template<class T>
inline auto optional_to_ptr(std::optional<T const> const& x)
    SDLRAII_BODY_EXP(x ? &*x : nullptr);
} // namespace impl

inline auto RenderCopy(Renderer* const renderer,
                       Texture* const texture,
                       std::optional<Rect const> const srcrect,
                       std::optional<Rect const> const dstrect)
    SDLRAII_BODY_EXP(RenderCopy(renderer,
                                texture,
                                impl::optional_to_ptr(srcrect),
                                impl::optional_to_ptr(dstrect)));

SDLRAII_WRAP_FN(RenderPresent, );
SDLRAII_WRAP_FN(SetRenderDrawColor, nonzero_error);
SDLRAII_WRAP_FN(RenderDrawLine, nonzero_error);
SDLRAII_WRAP_FN(RenderDrawLines, nonzero_error);
SDLRAII_WRAP_FN(RenderDrawPoint, nonzero_error);
SDLRAII_WRAP_FN(RenderDrawPoints, nonzero_error);
SDLRAII_WRAP_FN(RenderDrawRects, nonzero_error);
SDLRAII_WRAP_FN(RenderFillRects, nonzero_error);
SDLRAII_WRAP_FN(RenderDrawRect, nonzero_error);
SDLRAII_WRAP_FN(RenderFillRect, nonzero_error);

inline auto RenderDrawRect(Renderer* const renderer, Rect const rect)
    SDLRAII_BODY_EXP(RenderDrawRect(renderer, &rect));

inline auto RenderFillRect(Renderer* const renderer, Rect const rect)
    SDLRAII_BODY_EXP(RenderFillRect(renderer, &rect));

struct rgba {
  Uint8 r, g, b, a;
};

inline MayError<void>
    SetRenderDrawColor(Renderer* const renderer, rgba const color) noexcept {
  auto const [r, g, b, a] = color;
  if(!SetRenderDrawColor(renderer, r, g, b, a).ok()) return sdl::getError();
  return {};
}

/**
 * Gets the current draw color. Returns an ~rgba~ object instead of using
 * out parameters.
 */
inline rgba GetRenderDrawColor(Renderer* const renderer) noexcept {
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

// require the explicit degree type
inline auto RenderCopyEx(Renderer* const renderer,
                         Texture* const texture,
                         Rect const* const src,
                         Rect const* const dst,
                         degrees<double const> const angle,
                         Point const* const center,
                         RendererFlip const flip) noexcept
    SDLRAII_DECL_RET(SDL_RenderCopyEx(renderer,
                                      texture,
                                      src,
                                      dst,
                                      angle.number,
                                      center,
                                      flip));

inline auto RenderCopyEx(Renderer* const renderer,
                         Texture* const texture,
                         std::optional<Rect const> const src,
                         std::optional<Rect const> const dst,
                         degrees<double const> const angle,
                         std::optional<Point const> const center,
                         RendererFlip const flip)
    SDLRAII_BODY_EXP(RenderCopyEx(renderer,
                                  texture,
                                  impl::optional_to_ptr(src),
                                  impl::optional_to_ptr(dst),
                                  angle,
                                  impl::optional_to_ptr(center),
                                  flip));

inline std::optional<sdl::Rect>
    IntersectRect(sdl::Rect const* const A, sdl::Rect const* const B) {
  std::optional<sdl::Rect> result{sdl::Rect{}};
  if(SDL_IntersectRect(A, B, &*result))
    return result;
  else
    return std::nullopt;
}
inline auto IntersectRect(Rect const A, Rect const B)
    SDLRAII_BODY_EXP(IntersectRect(&A, &B));

SDLRAII_WRAP_FN(HasIntersection, );
inline auto HasIntersection(Rect const A, Rect const B)
    SDLRAII_BODY_EXP(HasIntersection(&A, &B));

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
inline std::optional<sdl::Event> NextEvent() noexcept {
  Event e;
  return SDL_PollEvent(&e) ? std::make_optional(e) : std::nullopt;
}

namespace BlendMode {
using type = SDL_BlendMode;
[[maybe_unused]] auto constexpr none = SDL_BLENDMODE_NONE;
[[maybe_unused]] auto constexpr blend = SDL_BLENDMODE_BLEND;
[[maybe_unused]] auto constexpr add = SDL_BLENDMODE_ADD;
[[maybe_unused]] auto constexpr mod = SDL_BLENDMODE_MOD;
} // namespace BlendMode

SDLRAII_WRAP_FN(SetTextureBlendMode, nonzero_error);
SDLRAII_WRAP_FN(SetSurfaceBlendMode, nonzero_error);
SDLRAII_WRAP_FN(SetRenderDrawBlendMode, nonzero_error);
SDLRAII_WRAP_FN(SetTextureAlphaMod, nonzero_error);
SDLRAII_WRAP_FN(SetTextureColorMod, nonzero_error);
SDLRAII_WRAP_FN(SetSurfaceAlphaMod, nonzero_error);
SDLRAII_WRAP_FN(SetSurfaceColorMod, nonzero_error);

SDLRAII_WRAP_GETTER(GetRenderDrawBlendMode, Renderer, BlendMode::type);
SDLRAII_WRAP_GETTER(GetTextureBlendMode, Texture, BlendMode::type);
SDLRAII_WRAP_GETTER(GetSurfaceBlendMode, Surface, BlendMode::type);
SDLRAII_WRAP_GETTER(GetTextureAlphaMod, Texture, Uint8);
SDLRAII_WRAP_GETTER(GetSurfaceAlphaMod, Surface, Uint8);

struct rgb {
  Uint8 r, g, b;
};

SDLRAII_WRAP_RGB_SETTER(SetSurfaceColorMod);
SDLRAII_WRAP_RGB_SETTER(SetTextureColorMod);

SDLRAII_WRAP_RGB_GETTER(GetSurfaceColorMod);
SDLRAII_WRAP_RGB_GETTER(GetTextureColorMod);

SDLRAII_WRAP_FN(SetWindowIcon, );
} // namespace sdl
#undef SDLRAII_THE_PREFIX

#endif // SDLRAII_SDL_INCLUDE_GUARD
