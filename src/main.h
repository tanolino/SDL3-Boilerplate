#include <SDL3/SDL.h>

#include <stdlib.h>
#include <stdio.h>

#include <nate_DynMemory.h>

// Shorthand types
typedef uint8_t u8;
typedef int8_t i8;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

//----------------------- Global shared state
extern SDL_Window* nate_window;
extern SDL_Renderer* nate_renderer;

extern SDL_AppResult nate_SDL_GetError(const char* function_name);

//----------------------- load externals (depend on memory.c)
extern bool nate_Load_File(const char* file_name, ByteBuffer* buffer);
extern SDL_Surface* nate_Load_Image(const ByteBuffer* array, MemoryOf3rd* stb_img_buffer);

// convinience function, but ineffective if we alloc/dealloc all the time
extern SDL_Surface* nate_Load_ImageFile(const char* file_name, MemoryOf3rd* stb_img_buffer);

//----------------------- tasks (depend on ArrayBuffer)
#define NATE_WITH_WORK_THREAD
typedef void(*nate_Task)(void* userdata);
extern bool nate_Task_Init();
extern bool nate_Task_Add(nate_Task function, void* userdata);
extern void nate_Task_Update(); // Start/Join/Manage Threads and Tasks
extern void nate_Task_Deinit();


