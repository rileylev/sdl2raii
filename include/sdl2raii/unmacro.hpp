#include "compat_macros.hpp"

// convert macros to inline functions
// this way template stuff works better

extern "C" {

#ifdef SDL_RWclose
static inline auto SDL_RWclose_(auto context)
    SDLRAII_BODY_EXP(SDL_RWclose(context))
#  undef SDL_RWclose
inline auto SDL_RWclose(struct SDL_RWops* context)
    SDLRAII_BODY_EXP(SDL_RWclose_(context))
#endif

#ifdef SDL_RWread
static inline auto
    SDL_RWread_(struct SDL_RWops* context, void* ptr, size_t size, size_t maxnum)
        SDLRAII_BODY_EXP(SDL_RWread(context, ptr, size, maxnum))
#  undef SDL_RWread
inline auto
    SDL_RWread(struct SDL_RWops* context, void* ptr, size_t size, size_t maxnum)
        SDLRAII_BODY_EXP(SDL_RWread_(context, ptr, size, maxnum))
#endif

#ifdef SDL_RWseek
static inline auto SDL_RWseek_(SDL_RWops* context, Sint64 offset, int whence)
    SDLRAII_BODY_EXP(SDL_RWseek(context, offset, whence))
#  undef SDL_RWseek
inline auto SDL_RWseek(SDL_RWops* context, Sint64 offset, int whence)
    SDLRAII_BODY_EXP(SDL_RWseek_(context, offset, whence))
#endif

#ifdef SDL_RWtell
static inline auto SDL_RWtell_(SDL_RWops* context)
    SDLRAII_BODY_EXP(SDL_RWtell(context))
#  undef SDL_RWtell
inline auto SDL_RWtell(SDL_RWops* context)
    SDLRAII_BODY_EXP(SDL_RWtell_(context))
#endif

#ifdef SDL_RWwrite
static inline auto SDL_RWwrite_(struct SDL_RWops* context,
                                const void* ptr,
                                size_t size,
                                size_t num)
    SDLRAII_BODY_EXP(SDL_RWwrite(context, ptr, size, num))
#  undef SDL_RWwrite
inline auto SDL_RWwrite(struct SDL_RWops* context,
                        const void* ptr,
                        size_t size,
                        size_t num)
    SDLRAII_BODY_EXP(SDL_RWwrite_(context, ptr, size, num))
#endif

#ifdef SDL_LoadBMP
static inline auto SDL_LoadBMP_(char const* file)
    SDLRAII_BODY_EXP(SDL_LoadBMP(file));
#  undef SDL_LoadBMP
inline auto SDL_LoadBMP(char const* file) SDLRAII_BODY_EXP(SDL_LoadBMP_(file));
#endif

#ifdef SDL_SaveBMP
static inline auto SDL_SaveBMP_(SDL_Surface* surface, const char* file)
    SDLRAII_BODY_EXP(SDL_SaveBMP(surface, file));
#  undef SDL_SaveBMP
inline auto SDL_SaveBMP(SDL_Surface* surface, char const* file)
    SDLRAII_BODY_EXP(SDL_SaveBMP_(surface, file));
#endif

#ifdef SDL_QuitRequested
static inline auto SDL_QuitRequested_(void)
    SDLRAII_BODY_EXP(SDL_QuitRequested())
#  undef SDL_QuitRequested
inline auto SDL_QuitRequested(void) SDLRAII_BODY_EXP(SDL_QuitRequested_())
#endif

#ifdef SDL_GetEventState
static inline auto SDL_GetEventState_(Uint32 type)
    SDLRAII_BODY_EXP(SDL_GetEventState(type))
#  undef SDL_GetEventState
inline auto SDL_GetEventState(Uint32 type)
    SDLRAII_BODY_EXP(SDL_GetEventState_(type))
#endif

#ifdef SDL_GameControllerAddMappingsFromFile
static inline auto SDL_GameControllerAddMappingsFromFile_(char const* file)
    SDLRAII_BODY_EXP(SDL_GameControllerAddMappingsFromFile(file))
#  undef SDL_GameControllerAddMappingsFromFile
inline auto SDL_GameControllerAddMappingsFromFile(char const* file)
    SDLRAII_BODY_EXP(SDL_GameControllerAddMappingsFromFile_(file))
#endif

#ifdef SDL_LoadWAV
static inline auto SDL_LoadWAV_(char const* file,
                                SDL_AudioSpec* spec,
                                Uint8** audio_buf,
                                Uint32* audio_len)
    SDLRAII_BODY_EXP(SDL_LoadWAV(file, spec, audio_buf, audio_len))
#  undef SDL_LoadWAV
inline auto SDL_LoadWAV(char const* file,
                        SDL_AudioSpec* spec,
                        Uint8** audio_buf,
                        Uint32* audio_len)
    SDLRAII_BODY_EXP(SDL_LoadWAV_(file, spec, audio_buf, audio_len))
#endif
}
