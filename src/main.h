#ifndef _NATE_CORE_FN
#define _NATE_CORE_FN

#include <SDL3/SDL.h>

#include <stdlib.h>
#include <stdio.h>

// For fopen errors
#include <errno.h>
#include <string.h>

#include <nate_DynMemory.h>

// Shorthand types
typedef uint8_t  u8;
typedef int8_t   i8;
typedef uint32_t u32;
typedef int32_t  i32;
typedef uint64_t u64;
typedef int64_t  i64;

//----------------------- Global shared state
extern SDL_Window*   nate_window;
extern SDL_Renderer* nate_renderer;

extern SDL_AppResult nate_SDL_GetError(const char* function_name);

//----------------------- load_externals.c
extern bool          nate_Load_File(const char* file_name, nate_ByteBuffer* buffer);
extern SDL_Surface*  nate_Load_Image(const nate_ByteBuffer* array, nate_MemoryOf3rd* stb_img_buffer);

typedef struct nate_LoadTextureContextTag
{
    SDL_Texture** result;
    SDL_Surface* surface;
} nate_LoadTextureContext;
#define nate_LoadImageContext0 (nate_LoadImageContext){0};
extern void nate_Load_Texture(nate_LoadTextureContext* context);

// ---------------------- text.c
extern bool          nate_Text_Init();
extern SDL_Surface*  nate_Text_Render(const char* text, nate_ByteBuffer* buffer);
extern SDL_Surface*  nate_Text_Render_Sized(const char* text, nate_ByteBuffer* buffer, float size);


// ---------------------- the actual game logic

// Init state
extern SDL_AppResult game_Init();

// Reacts to events
extern SDL_AppResult game_Event(const SDL_Event* ev);

// Process timed events and Draw the Screen
extern SDL_AppResult game_Iterate(Uint64 ticks);

// Clean up
extern void game_Quit();

#endif // _NATE_CORE_FN
