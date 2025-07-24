#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
// 1 << 14 = 16384
#define STBI_MAX_DIMENSIONS (1 << 14)
#include <stb_image.h>

bool nate_Load_File(const char *filename, ByteBuffer* buffer)
{
    if (!buffer) return false;

    FILE* file = fopen(filename, "r");
    if(!file) {

        fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
        return false;
    }

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    // 1 << 25 = 32
    if (file_size <= 0 ||
        file_size > (1 << 25))
    {
        fprintf(stderr, "Illegal file size: %d bytes\n", file_size);
        goto JmpFailure;
    }
    
    //printf("Allocate %d for file to load\n", file_size);
    if (!nate_ByteBuffer_Alloc(buffer, file_size)) {
        goto JmpFailure;
    }

    int read_bytes = fread(buffer->data, sizeof(char), file_size, file);
    if (read_bytes != file_size)
    {
        fprintf(stderr, "Failed to read %d bytes from file, got only %d\n", file_size, read_bytes);
        goto JmpFailure;
    }
    buffer->size = file_size;
    fclose(file);
    return true;
    
    JmpFailure:
    if(file) fclose(file);
    return false;
}

SDL_Surface* nate_Load_Image(const ByteBuffer* buffer, MemoryOf3rd* stb_img_buffer)
{
    SDL_Surface* surface = NULL;
    if (!buffer || !buffer->data || !buffer->size) return surface;
    if (!stb_img_buffer) return surface;

    int width = 0, height = 0, comp = 0;
    stb_img_buffer->data = (void*)stbi_load_from_memory((stbi_uc*)buffer->data, buffer->size,
                                   &width, &height, &comp, 0);
    stb_img_buffer->free_fn = stbi_image_free;
    if (!stb_img_buffer->data) {
        fprintf(stderr, "Failed to load image from memory\n");
        return surface;
    }

    if (comp == 4) {
        surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32,
                                     stb_img_buffer->data, 4 * width);
    } else if (comp == 3) {
        surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGB24,
                                     stb_img_buffer->data, 3 * width);
    } else {
        fprintf(stderr, "images with %d color channels not yet implemented.\n", comp);
        return surface;
    }
    if (!surface) {
        nate_SDL_GetError("SDL_CreateSurfaceFrom");
    }
    return surface;
}

SDL_Surface* nate_Load_ImageFile(const char *filename, MemoryOf3rd* stb_img_buffer)
{
    SDL_Surface* res = NULL;
    ByteBuffer buffer = ByteBuffer0;
    if (nate_Load_File(filename, &buffer)) {
        res = nate_Load_Image(&buffer, stb_img_buffer);
    }
    nate_ByteBuffer_Free(&buffer);
    return res;
}
