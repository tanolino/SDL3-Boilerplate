#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

static SDL_Window* window = nullptr;
static SDL_GPUDevice* device = nullptr;

inline static SDL_AppResult sShorthand_SDL_GetError(const char* function_name) {
    fprintf(stderr, "%s failed:\n%s\n", function_name, SDL_GetError());
    return SDL_APP_FAILURE;
}

/* SDL Callback functions */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_CreateTexture()
    // Create the window
    window = SDL_CreateWindow("First Persom", 960, 540, SDL_WINDOW_RESIZABLE);
    if (!window)
        return sShorthand_SDL_GetError("SDL_CreateWindow(...)");

    // create the device
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
    if (!device)
        sShorthand_SDL_GetError("SDL_CreateGPUDevice(...)");
    if (!SDL_ClaimWindowForGPUDevice(device, window))
        sShorthand_SDL_GetError("SDL_ClaimWindowForGPUDevice(...)");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    // acquire the command buffer
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

    // get the swapchain texture
    SDL_GPUTexture* swapchainTexture;
    Uint32 width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height);

    // end the frame early if a swapchain texture is not available
    if (swapchainTexture == NULL)
    {
        // you must always submit the command buffer
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return SDL_APP_CONTINUE;
    }

    // create the color target
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = { 240 / 255.0f, 120 / 255.0f, 120 / 255.0f, 255 / 255.0f };
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    // begin a render pass
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

    // draw something

    // end the render pass
    SDL_EndGPURenderPass(renderPass);

    // submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (device)
        SDL_DestroyGPUDevice(device);

    if (window)
        SDL_DestroyWindow(window);
}