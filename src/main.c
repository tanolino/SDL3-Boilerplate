#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#define NATE_DYNMEMORY_IMPLEMENTATION
#include <nate_DynMemory.h>

#define NATE_TASK_IMPLEMENTATION
#define NATE_TASK_SDL3_THREAD
#include <nate_Task.h>

#include "main.h"

// Global shared state
SDL_Window* nate_window = NULL;
SDL_Renderer* nate_renderer = NULL;

SDL_AppResult nate_SDL_GetError(const char* function_name) {
    fprintf(stderr, "%s failed:\n%s\n", function_name, SDL_GetError());
    return SDL_APP_FAILURE;
}
// Local state
static Uint64 s_last_tick;

// Update tick, returns diff
static Uint64 update_tick()
{
    Uint64 res;
    Uint64 new_tick = SDL_GetTicksNS();
    if (new_tick > s_last_tick)
        res = new_tick - s_last_tick;
    else
        res = s_last_tick - new_tick;
    s_last_tick = new_tick;
    return res;
}

static SDL_Texture* image = NULL;
static SDL_Texture* text = NULL;

typedef struct SurfaceToTexture {
    SDL_Surface* surface;
    SDL_Texture** texture;
} SurfaceToTexture;

static void s_load_image_sync(void* ptr)
{
    if (!ptr) return;
    
    SurfaceToTexture* src = (SurfaceToTexture*)ptr;
    if (src && src->surface && src->texture)
        (*src->texture) = SDL_CreateTextureFromSurface(nate_renderer, src->surface);
    free(ptr);
}

static void s_load_image_async(void* ptr)
{
    if (!ptr) return;

    MemoryOf3rd tmp_mem = MemoryOf3rd0;
    const char* file_name = (const char*)ptr;
    SDL_Surface* surface = nate_Load_ImageFile(file_name, &tmp_mem);
    SurfaceToTexture* s_to_t = NULL;
    if (surface) {
        //printf("successfully loaded file \"%s\"\n", file_name);
        s_to_t = (SurfaceToTexture*)malloc(sizeof(SurfaceToTexture));
        if (s_to_t) {
            s_to_t->surface = surface;
            s_to_t->texture = &image;
            SDL_RunOnMainThread(s_load_image_sync, (void*)s_to_t, true);
            SDL_DestroySurface(surface);
        }
    }
    nate_MemoryOf3rd_Free(&tmp_mem);
}

static void s_load_text_async(void *ptr)
{
    ByteBuffer tmp_mem = ByteBuffer0;
    SDL_Surface* surface = nate_Text_Render("Das ist Text", &tmp_mem);
    SurfaceToTexture* s_to_t = NULL;
    if (surface) {
        s_to_t = (SurfaceToTexture*)malloc(sizeof(SurfaceToTexture));
        if (s_to_t) {
            s_to_t->surface = surface;
            s_to_t->texture = &text;
            SDL_RunOnMainThread(s_load_image_sync, (void*)s_to_t, true);
            SDL_DestroySurface(surface);
        }
    }
    else {
        fprintf(stderr, "Failed to create text SDL_Surface\n");
    }
    nate_ByteBuffer_Free(&tmp_mem);
}

/* SDL Callback functions */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // Init Subsystems
    //int result = SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    //if (result < 0)
    //    return nate_SDL_GetError("SDL_InitSubsystem(...)");
    
    // Create the window
    nate_window = SDL_CreateWindow("First Persom", 960, 540, SDL_WINDOW_RESIZABLE);
    if (!nate_window)
        return nate_SDL_GetError("SDL_CreateWindow(...)");

    nate_renderer = SDL_CreateRenderer(nate_window, NULL);
    if (!nate_renderer)
        return nate_SDL_GetError("SDL_CreateRenderer(...)");
    SDL_SetRenderVSync(nate_renderer, 1);

    if(!nate_Task_Init())
        return SDL_APP_FAILURE;
    
    if (!nate_Text_Init())
        return SDL_APP_FAILURE;

    s_last_tick = SDL_GetTicksNS();
    nate_Task_Add(s_load_image_async, "test.png");
    nate_Task_Add(s_load_text_async, NULL);
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    Uint64 delta_tick = update_tick();
    SDL_SetRenderDrawColor(nate_renderer, 200, 20, 20, 255);
    SDL_RenderClear(nate_renderer);

    if (image)
        SDL_RenderTexture(nate_renderer, image, NULL, NULL);
    if (text) {
        SDL_FRect dstrect = {0};
        dstrect.w = text->w;
        dstrect.h = text->h;
        SDL_RenderTexture(nate_renderer, text, NULL, &dstrect);
    }
    
    SDL_RenderPresent(nate_renderer);
    nate_Task_Update();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    nate_Task_Deinit();
    if (image)
        SDL_DestroyTexture(image);
    if (nate_renderer)
        SDL_DestroyRenderer(nate_renderer);
    if (nate_window)
        SDL_DestroyWindow(nate_window);
}
