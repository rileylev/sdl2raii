#ifndef SDLRAII_SDL_INCLUDE_GUARD
#define SDLRAII_SDL_INCLUDE_GUARD

#include "compat_macros.hpp"
#include "MayError.hpp"
#define SDLRAII_THE_PREFIX SDL
#include "wrapgen_macros.hpp"

#include <SDL2/SDL.h>
#include "unmacro.hpp"

#include <memory>
#include <tuple>
#include <optional>

namespace sdl {

// RWops
// https://wiki.libsdl.org/SDL_RWops
SDLRAII_WRAP_TYPE(RWops);
SDLRAII_DEFUNIQUE(RWops, SDL_RWclose);
SDLRAII_WRAP_MAKER(UniqueRWops, RWFromConstMem);
SDLRAII_WRAP_MAKER(UniqueRWops, RWFromFile);
SDLRAII_WRAP_MAKER(UniqueRWops, RWFromFP);
SDLRAII_WRAP_MAKER(UniqueRWops, RWFromMem);

// TODO
// Should I be returning void* vs byte* or something more typed?

struct LoadFileData {
  void* data  = nullptr;
  size_t size = 0;

  LoadFileData() = default;

  LoadFileData(LoadFileData const&) = delete;
  LoadFileData(LoadFileData&& other) : data{other.data}, size{other.size} {
    other.data = nullptr;
  }

  ~LoadFileData() { SDL_free(data); }

  void* release() noexcept {
    void* data_copy = data;
    data            = nullptr;
    return data_copy;
  }
};

inline MayError<LoadFileData> LoadFile_RW(RWops* src) noexcept {
  LoadFileData d;
  d.data = SDL_LoadFile_RW(src, &d.size, false);
  if(d.data == nullptr) return sdl::GetError();
  return d;
}
inline auto LoadFile_RW(UniqueRWops src)
    SDLRAII_BODY_EXP(LoadFile_RW(src.get()))

inline MayError<LoadFileData> LoadFile(const char* file) {
  auto rw = RWFromFile(file, "rb");
  SDLRAII_BAIL_ERROR(rw);
  return LoadFile_RW(rw.success().get());
}

// read endian functions
SDLRAII_WRAP_FN(ReadU8, );
SDLRAII_WRAP_FN(ReadLE16, );
SDLRAII_WRAP_FN(ReadBE16, );
SDLRAII_WRAP_FN(ReadLE32, );
SDLRAII_WRAP_FN(ReadBE32, );
SDLRAII_WRAP_FN(ReadLE64, );
SDLRAII_WRAP_FN(ReadBE64, );

SDLRAII_WRAP_FN(WriteU8, );
SDLRAII_WRAP_FN(WriteLE16, );
SDLRAII_WRAP_FN(WriteBE16, );
SDLRAII_WRAP_FN(WriteLE32, );
SDLRAII_WRAP_FN(WriteBE32, );
SDLRAII_WRAP_FN(WriteLE64, );
SDLRAII_WRAP_FN(WriteBE64, );

// surface
SDLRAII_WRAP_TYPE(Surface);
SDLRAII_DEFUNIQUE(Surface, SDL_FreeSurface);
SDLRAII_WRAP_MAKER(UniqueSurface, LoadBMP);

SDLRAII_WRAP_FN(SetSurfaceBlendMode, nonzero_error);
SDLRAII_WRAP_FN(SetSurfaceAlphaMod, nonzero_error);
SDLRAII_WRAP_FN(SetSurfaceColorMod, nonzero_error);

namespace BlendMode {
using type                            = SDL_BlendMode;
[[maybe_unused]] auto constexpr none  = SDL_BLENDMODE_NONE;
[[maybe_unused]] auto constexpr blend = SDL_BLENDMODE_BLEND;
[[maybe_unused]] auto constexpr add   = SDL_BLENDMODE_ADD;
[[maybe_unused]] auto constexpr mod   = SDL_BLENDMODE_MOD;
} // namespace BlendMode

SDLRAII_WRAP_GETTER(GetSurfaceBlendMode, Surface, BlendMode::type);
SDLRAII_WRAP_GETTER(GetSurfaceAlphaMod, Surface, Uint8);

// render
//

SDLRAII_WRAP_TYPE(Window);
SDLRAII_WRAP_TYPE(Renderer);
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
SDLRAII_DEFUNIQUE(Texture, SDL_DestroyTexture);

SDLRAII_WRAP_MAKER(UniqueWindow, CreateWindow);
SDLRAII_WRAP_MAKER(UniqueRenderer, CreateRenderer);
SDLRAII_WRAP_MAKER(UniqueTexture, CreateTextureFromSurface);

template<class T>
inline auto CreateWindowFrom(T* data) SDLRAII_BODY_EXP(
    UniqueWindow(SDL_CreateWindowFrom(static_cast<void*>(data))));

/**
 * Creates a texture from a ~UniqueSurface~. Deletes the ~UniqueSurface~
 * after.
 */
inline sdl::MayError<UniqueTexture>
    CreateTextureFromSurface(Renderer* const renderer, UniqueSurface surface) {
  return CreateTextureFromSurface(renderer, surface.get());
}

inline Point GetRendererOutputSize(Renderer * const renderer){
  // i believe GetRendererOutputSize doesn't modify renderer, but isn't marked const
  Point p;
  SDL_GetRendererOutputSize(renderer, &p.x,&p.y);
  return p;
}

/**
 * SDL_FLIP_... as named constants
 */
namespace flip {
enum flags : Uint32 {
  none       = SDL_FLIP_NONE,
  horizontal = SDL_FLIP_HORIZONTAL,
  vertical   = SDL_FLIP_VERTICAL
};
} // namespace flip

namespace window {
[[maybe_unused]] constexpr auto pos_undefined = SDL_WINDOWPOS_UNDEFINED;
[[maybe_unused]] constexpr auto pos_centered  = SDL_WINDOWPOS_CENTERED;
enum flags : Uint32 {
  fullscreen         = SDL_WINDOW_FULLSCREEN,
  fullscreen_desktop = SDL_WINDOW_FULLSCREEN_DESKTOP,
  opengl             = SDL_WINDOW_OPENGL,
  shown              = SDL_WINDOW_SHOWN,
  hidden             = SDL_WINDOW_HIDDEN,
  borderless         = SDL_WINDOW_BORDERLESS,
  resizable          = SDL_WINDOW_RESIZABLE,
  minimized          = SDL_WINDOW_MINIMIZED,
  maximized          = SDL_WINDOW_MAXIMIZED,
  input_grabbed      = SDL_WINDOW_INPUT_GRABBED,
  input_focus        = SDL_WINDOW_INPUT_FOCUS,
  mouse_focus        = SDL_WINDOW_MOUSE_FOCUS,
  foreign            = SDL_WINDOW_FOREIGN,
  allow_highdpi      = SDL_WINDOW_ALLOW_HIGHDPI,
  mouse_capture      = SDL_WINDOW_MOUSE_CAPTURE,
  always_on_top      = SDL_WINDOW_ALWAYS_ON_TOP,
  skip_taskbar       = SDL_WINDOW_SKIP_TASKBAR,
  utility            = SDL_WINDOW_UTILITY,
  tooltip            = SDL_WINDOW_TOOLTIP,
  popup_menu         = SDL_WINDOW_POPUP_MENU
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
    return sdl::GetError();
  return std::tuple{UniqueWindow{win}, UniqueRenderer{ren}};
}

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

SDLRAII_WRAP_FN(RenderCopy, nonzero_error);
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

namespace renderer {
enum flags : Uint32 {
  software      = SDL_RENDERER_SOFTWARE,
  accelerated   = SDL_RENDERER_ACCELERATED,
  presentvsync  = SDL_RENDERER_PRESENTVSYNC,
  targettexture = SDL_RENDERER_TARGETTEXTURE
};
}

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
  SDLRAII_BAIL_ERROR(SetRenderDrawColor(renderer, r, g, b, a));
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
                         RendererFlip const flip)
    SDLRAII_BODY_EXP(nonzero_error(SDL_RenderCopyEx(renderer,
                                                    texture,
                                                    src,
                                                    dst,
                                                    angle.number,
                                                    center,
                                                    flip)));

template<class Rect, class Point>
inline auto RenderCopyEx(Renderer* const renderer,
                         Texture* const texture,
                         std::optional<Rect const> const src,
                         std::optional<Rect const> const dst,
                         degrees<double> const angle,
                         std::optional<Point const> const center,
                         RendererFlip const flip)
    SDLRAII_BODY_EXP(RenderCopyEx(renderer,
                                  texture,
                                  impl::optional_to_ptr(src),
                                  impl::optional_to_ptr(dst),
                                  angle,
                                  impl::optional_to_ptr(center),
                                  flip));

SDLRAII_WRAP_FN(SetTextureBlendMode, nonzero_error);
SDLRAII_WRAP_FN(SetRenderDrawBlendMode, nonzero_error);
SDLRAII_WRAP_FN(SetTextureAlphaMod, nonzero_error);
SDLRAII_WRAP_FN(SetTextureColorMod, nonzero_error);

SDLRAII_WRAP_GETTER(GetRenderDrawBlendMode, Renderer, BlendMode::type);
SDLRAII_WRAP_GETTER(GetTextureBlendMode, Texture, BlendMode::type);
SDLRAII_WRAP_GETTER(GetTextureAlphaMod, Texture, Uint8);

struct rgb {
  Uint8 r, g, b;
};

SDLRAII_WRAP_RGB_SETTER(SetSurfaceColorMod);
SDLRAII_WRAP_RGB_GETTER(GetSurfaceColorMod);
SDLRAII_WRAP_RGB_SETTER(SetTextureColorMod);
SDLRAII_WRAP_RGB_GETTER(GetTextureColorMod);

SDLRAII_WRAP_FN(SetWindowIcon, );

SDLRAII_WRAP_FN(RenderClear, nonzero_error);
// couldn't find the error in the doc comment
SDLRAII_WRAP_FN(RenderSetScale, nonzero_error);

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

SDLRAII_WRAP_FN(RectEmpty, )
inline auto RectEmpty(Rect const r) SDLRAII_BODY_EXP(RectEmpty(&r))

SDLRAII_WRAP_FN(PointInRect, )
inline auto PointInRect(Point const p, Rect const r)
    SDLRAII_BODY_EXP(PointInRect(&p, &r))

// floating point rects
SDLRAII_WRAP_TYPE(FRect);
SDLRAII_WRAP_TYPE(FPoint);
SDLRAII_WRAP_RENAME_FN(RenderDrawLine, SDL_RenderDrawLineF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderDrawLines, SDL_RenderDrawLinesF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderDrawPoint, SDL_RenderDrawPointF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderDrawPoints, SDL_RenderDrawPointsF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderDrawRects, SDL_RenderDrawRectsF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderFillRects, SDL_RenderFillRectsF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderDrawRect, SDL_RenderDrawRectF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderFillRect, SDL_RenderFillRectF, nonzero_error);
SDLRAII_WRAP_RENAME_FN(RenderCopy, SDL_RenderCopyF, nonzero_error);

inline auto RenderCopyEx(Renderer* const renderer,
                         Texture* const texture,
                         Rect const* const src,
                         FRect const* const dst,
                         degrees<double const> const angle,
                         FPoint const* const center,
                         RendererFlip const flip) noexcept
    SDLRAII_DECL_RET(SDL_RenderCopyExF(renderer,
                                       texture,
                                       src,
                                       dst,
                                       angle.number,
                                       center,
                                       flip));

inline auto RenderSetScale(sdl::Renderer* renderer, sdl::FPoint scale)
    SDLRAII_BODY_EXP(sdl::RenderSetScale(renderer, scale.x, scale.y));
inline sdl::FPoint RenderGetScale(sdl::Renderer* renderer) noexcept {
  float sx, sy;
  SDL_RenderGetScale(renderer, &sx, &sy);
  return {sx, sy};
}

SDLRAII_WRAP_FN(Init, nonzero_error);
SDLRAII_WRAP_FN(Quit, );

struct Quitter {
  ~Quitter() { sdl::Quit(); }
};

namespace init {
using flags                                     = Uint32;
using type                                      = Uint32;
[[maybe_unused]] constexpr flags timer          = SDL_INIT_TIMER;
[[maybe_unused]] constexpr flags audio          = SDL_INIT_AUDIO;
[[maybe_unused]] constexpr flags video          = SDL_INIT_VIDEO;
[[maybe_unused]] constexpr flags joystick       = SDL_INIT_JOYSTICK;
[[maybe_unused]] constexpr flags haptic         = SDL_INIT_HAPTIC;
[[maybe_unused]] constexpr flags gamecontroller = SDL_INIT_GAMECONTROLLER;
[[maybe_unused]] constexpr flags events         = SDL_INIT_EVENTS;
[[maybe_unused]] constexpr flags everything     = SDL_INIT_EVERYTHING;
} // namespace init

[[nodiscard]] MayError<Quitter>
    ScopedInit(init::flags subsystems = {}) noexcept {
  auto const result = sdl::Init(subsystems);
  SDLRAII_BAIL_ERROR(result);
  return Quitter{};
}

SDLRAII_WRAP_TYPE(Event);
inline bool HasNextEvent() noexcept { return SDL_PollEvent(nullptr); }
inline std::optional<sdl::Event> NextEvent() noexcept {
  Event e;
  return SDL_PollEvent(&e) ? std::make_optional(e) : std::nullopt;
}

} // namespace sdl
#undef SDLRAII_THE_PREFIX

#endif // SDLRAII_SDL_INCLUDE_GUARD
