#include "main.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <dareo.regular.h>

static stbtt_fontinfo s_fontinfo = {0};
static int s_font_size = 20;
static float s_font_scale = 1.f;
// Talk to the outside world

bool nate_Text_Init()
{
    int off = stbtt_GetFontOffsetForIndex(dareo_regular_ttf, 0);
    if (!stbtt_InitFont(&s_fontinfo, dareo_regular_ttf, off))
        return false;

    s_font_scale = stbtt_ScaleForPixelHeight(&s_fontinfo, s_font_size);
    int ix0, ix1, iy0, iy1;
    stbtt_GetFontBoundingBox(&s_fontinfo, &ix0, &iy0, &ix1, &iy1);
    printf("Font bounding box %i|%i %i|%i\n", ix0, iy0, ix1, iy1);
    printf("Font scale %f\n", s_font_scale);
    return true;
}

SDL_Surface* nate_Text_Render(const char* text, ByteBuffer* buffer)
{

    // stbtt_GetCodepointBitmap(&s_fontinfo, 0.f, s_font_scale, 'c', w, h, wOff, yOff);
    return NULL;
}

