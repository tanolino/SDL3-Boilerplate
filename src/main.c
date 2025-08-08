#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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

static nate_ByteBuffer tmp_mem = nate_ByteBuffer0;
static SDL_Texture* image = NULL;
static SDL_Texture* text = NULL;
static float text_x = 70.f, text_y = 70.f;

static void s_load_textures()
{
    SDL_Surface* surface = nate_Text_Render("Das ist Text", &tmp_mem);
    if (surface) {
        nate_LoadTextureContext ctx = {0};
        ctx.surface = surface;
        ctx.result = &text;
        SDL_RunOnMainThread((SDL_MainThreadCallback)nate_Load_Texture, (void*)&ctx, true);
        SDL_DestroySurface(surface);
    }
    else {
        fprintf(stderr, "Failed to create text SDL_Surface\n");
    }
    
    if (nate_Load_File("test.png", &tmp_mem)){
        nate_MemoryOf3rd rdmem = nate_MemoryOf3rd0;
        if ((surface = nate_Load_Image(&tmp_mem,  &rdmem)) != NULL) {
            nate_LoadTextureContext ctx = {0};
            ctx.surface = surface;
            ctx.result = &image;
            SDL_RunOnMainThread((SDL_MainThreadCallback)nate_Load_Texture, (void*)&ctx, true);
            SDL_DestroySurface(surface);
        }
    }
    else {
        fprintf(stderr, "Failed to load test.png .\n");
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
    
    if (!nate_Text_Init())
        return SDL_APP_FAILURE;
    s_load_textures();

    s_last_tick = SDL_GetTicksNS();
    
    // return SDL_APP_CONTINUE;
    return game_Init();
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return SDL_APP_SUCCESS;
	case SDL_EVENT_MOUSE_MOTION:
	    text_x = event->motion.x;
	    text_y = event->motion.y;
    }

    // return SDL_APP_CONTINUE;
    return game_Event(event);
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    Uint64 delta_ticks = update_tick();
    SDL_SetRenderDrawColor(nate_renderer, 220, 120, 120, 255);
    SDL_RenderClear(nate_renderer);

    if (image)
        SDL_RenderTexture(nate_renderer, image, NULL, NULL);
    if (text) {
        SDL_FRect dstrect = {0};
        dstrect.x = text_x;
        dstrect.y = text_y;
        dstrect.w = text->w + 6;
        dstrect.h = text->h + 6;
        SDL_RenderFillRect(nate_renderer, &dstrect);
        dstrect.x += 3;
        dstrect.y += 3;
        dstrect.w = text->w;
        dstrect.h = text->h;
        SDL_RenderTexture(nate_renderer, text, NULL, &dstrect);
    }

    const SDL_AppResult res = game_Iterate(delta_ticks);
    SDL_RenderPresent(nate_renderer);
    return res; // SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (image)
        SDL_DestroyTexture(image);
    if (nate_renderer)
        SDL_DestroyRenderer(nate_renderer);
    if (nate_window)
        SDL_DestroyWindow(nate_window);
}
