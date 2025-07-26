#include "main.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <dareo.regular.h>

static stbtt_fontinfo s_fontinfo = {0};
static struct {
int size;
int max_width;
int max_height;
int y_lead;
float scale; // depends on s_font_size
} s_font =  {
    .size = 40,
    .max_width = 1,
    .max_height = 1,
    .y_lead = 0,
    .scale = 1.f,
};

static void s_set_font_size(int size)
{
    if (size <= 0) return;
    
    s_font.size = size;
    s_font.scale = stbtt_ScaleForPixelHeight(&s_fontinfo, s_font.size);
    
    int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
    stbtt_GetFontBoundingBox(&s_fontinfo, &x0, &y0, &x1, &y1);
    s_font.max_width = (x1 - x0) * s_font.scale + 2; // 1 for rounding + 1 for subpixel
    s_font.max_height = (y1 - y0) * s_font.scale + 1; // 1 for rounding error
    // printf("ROOT: %i %i\n", y0, y1);
    s_font.y_lead = y1;
}

// Talk to the outside world

bool nate_Text_Init()
{
    int off = stbtt_GetFontOffsetForIndex(dareo_regular_ttf, 0);
    if (!stbtt_InitFont(&s_fontinfo, dareo_regular_ttf, off))
        return false;

    s_set_font_size(s_font.size);
    return true;
}

SDL_Surface* nate_Text_Render(const char* text, ByteBuffer* buffer)
{
    if (!text) return NULL;
    
    const char* t = text;
    size_t final_width = 0;
    while(*t != '\0') {
        int x0 = 0, x1 = 0, y0 = 0, y1 = 0, adv = 0, bear = 0;
        stbtt_GetCodepointBitmapBox(&s_fontinfo, *t, 1.f, 1.f, &x0, &y0, &x1, &y1);
        final_width += x1 - x0;
        stbtt_GetCodepointHMetrics(&s_fontinfo, *t, &adv, &bear);
        final_width += adv + bear;
        t++;
    }
    if (!final_width) {
        fprintf(stderr, "Failed to get width of string \"%s\"", text);
        return NULL;
    }
    // printf("Final width: %f\n", final_width * s_font.scale);
    final_width = (int)(final_width * s_font.scale) + 1;

    if (!nate_ByteBuffer_Alloc(buffer, s_font.max_height * final_width * 4))
        return NULL;
    buffer->size = s_font.max_height * final_width * 4;
    memset(buffer->data, 0, buffer->size);

    unsigned char* tmpbuff = (unsigned char*)malloc(s_font.max_width * s_font.max_height);
    if (!tmpbuff) {
        fprintf(stderr, "Failed to allocate %i bytes temporarly\n",
                s_font.max_width * s_font.max_height);
        return NULL;
    }

    size_t h_offset = 0;
    t = text;
    while(*t != '\0') {
        int x0 = 0, x1 = 0, y0 = 0, y1 = 0, adv = 0, bear = 0;
        stbtt_GetCodepointBitmapBox(&s_fontinfo, *t, 1.f, 1.f, &x0, &y0, &x1, &y1);
        stbtt_GetCodepointHMetrics(&s_fontinfo, *t, &adv, &bear);
        float h_subpixel = (bear + x0) * s_font.scale;
        {
            int h_shift = (int)h_subpixel;
            h_subpixel -= h_shift;
            h_offset += h_shift;
        }
        float v_subpixel = (s_font.y_lead + y0) * s_font.scale;
        int v_shift = (int)(v_subpixel);
        v_subpixel -= v_shift;
        memset(tmpbuff, 0, s_font.max_width * s_font.max_height);
        stbtt_MakeCodepointBitmapSubpixel(&s_fontinfo, tmpbuff,
                                  s_font.max_width, s_font.max_height, s_font.max_width,
                                  s_font.scale, s_font.scale, h_subpixel, v_subpixel,*t);
        
        {
            float width = (x1 - x0) * s_font.scale + 1;
            float height = (y1 - y0) * s_font.scale + 1;
            unsigned char* dOff = (unsigned char*)buffer->data + h_offset * 4;
            int y_max = (int)height + 1;
            if (s_font.max_height < y_max)
                y_max = s_font.max_width;
            int x_max = (int)width + 1;
            if (s_font.max_width < x_max)
                x_max = s_font.max_height;
        
            const unsigned char* from = (unsigned char*)buffer->data;
            const unsigned char* onto = (unsigned char*)buffer->data + buffer->size;
            for (int y = 0; y < y_max; y++) {
                int dy = y + v_shift;
                unsigned char* dRowStart = dOff + (dy * final_width * 4);
                unsigned char* sRowStart = tmpbuff + (y * s_font.max_width);
                for (int x = 0; x < x_max; x++) {
                    unsigned char* dst = dRowStart + (4 * x + 3);
                    if (dst >= from && dst < onto) // Range check ... JUST in case
                    {
                        int num = (int)*dst + (int)sRowStart[x];
                        if (num > 255)
                            *dst = 255;
                        else
                            *dst = (unsigned char)num;
                    }
                }
            }
        }

        h_subpixel += adv * s_font.scale;
        {
            int h_shift = (int)h_subpixel;
            h_subpixel -= h_shift;
            h_offset += h_shift;
        }
        t++;
    }
    free(tmpbuff);

    return SDL_CreateSurfaceFrom(final_width, s_font.max_height,
                                 SDL_PIXELFORMAT_RGBA32, buffer->data, final_width * 4);
}

