#include <SDL3/SDL.h>

#include <stdlib.h>
#include <stdio.h>

// For fopen errors
#include <errno.h>
#include <string.h>

#include <nate_DynMemory.h>
#include <nate_Task.h>

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
extern bool          nate_Load_File(const char* file_name, ByteBuffer* buffer);
extern SDL_Surface*  nate_Load_Image(const ByteBuffer* array, MemoryOf3rd* stb_img_buffer);

// convinience function, but ineffective if we alloc/dealloc all the time
extern SDL_Surface*  nate_Load_ImageFile(const char* file_name, MemoryOf3rd* stb_img_buffer);

// ---------------------- text.c
extern bool          nate_Text_Init();
extern SDL_Surface*  nate_Text_Render(const char* text, ByteBuffer* buffer);


