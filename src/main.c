#include "SDL3/SDL_clipboard.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

inline static SDL_AppResult sShorthand_SDL_GetError(const char* function_name) {
    fprintf(stderr, "%s failed:\n%s\n", function_name, SDL_GetError());
    return SDL_APP_FAILURE;
}

/* SDL Callback functions */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // Init Subsystems
    int result = 0;
    if ((result = SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) < 0)
        return sShorthand_SDL_GetError("SDL_InitSubsystem(...)");
    
    // Create the window
    window = SDL_CreateWindow("First Persom", 960, 540, SDL_WINDOW_RESIZABLE);
    if (!window)
        return sShorthand_SDL_GetError("SDL_CreateWindow(...)");

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
        return sShorthand_SDL_GetError("SDL_CreateRenderer(...)");
    SDL_SetRenderVSync(renderer, 2);
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);
}
