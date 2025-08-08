#include "main.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <dareo.regular.h>

static struct nate_FontExt {
    int size;
    int max_width;
    int max_height;
    int y_lead;
    float scale; // depends on s_font_size
    stbtt_fontinfo info;
} s_font_default =  {
    .info = NULL,
    .size = 40,
    .max_width = 1,
    .max_height = 1,
    .y_lead = 0,
    .scale = 1.f,
};
typedef struct nate_FontExt nate_FontExt;

static void s_set_font_size(int size, nate_FontExt* f)
{
    if (size <= 0) return;
    
    f->size = size;
    f->scale = stbtt_ScaleForPixelHeight(&f->info, f->size);
    
    int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
    stbtt_GetFontBoundingBox(&f->info, &x0, &y0, &x1, &y1);
    f->max_width = (x1 - x0) * f->scale + 2; // 1 for rounding + 1 for subpixel
    f->max_height = (y1 - y0) * f->scale + 1; // 1 for rounding error
    // printf("ROOT: %i %i\n", y0, y1);
    f->y_lead = y1;
}

// Talk to the outside world

bool nate_Text_Init()
{
    int off = stbtt_GetFontOffsetForIndex(dareo_regular_ttf, 0);
    if (!stbtt_InitFont(&s_font_default.info, dareo_regular_ttf, off))
        return false;

    s_set_font_size(s_font_default.size, &s_font_default);
    return true;
}

static size_t s_final_text_width_unscaled(const char* text, nate_FontExt* f)
{
    size_t res = 0;
    int adv = 0, bear = 0;
    bool first = true;
    
    if (!text) return res;
    while(*text != '\0')
    {
        stbtt_GetCodepointHMetrics(&f->info, *text, &adv, &bear);
        if (first && adv < 0)
            adv = -adv;
        res += adv + bear;
        text++;
        first = false;
    }
    return res;
}

typedef struct nate_PrintOneCharTag {
    unsigned char* tmp;
    size_t tmp_width;
    size_t tmp_height;
    unsigned char* out;
    unsigned char* out_end;
    size_t out_width;
    size_t h_offset;
    nate_FontExt* font;
} nate_PrintOneChar;

static void s_plot_one_char_inner(const char c, nate_PrintOneChar* d)
{
    int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
    stbtt_GetCodepointBitmapBox(&d->font->info, c, 1.f, 1.f, &x0, &y0, &x1, &y1);
    
    float h_subpixel = d->font->scale * (d->h_offset + x0);
    int h_shift = (int)h_subpixel;
    h_subpixel -= h_shift;

    float v_subpixel = d->font->scale * (d->font->y_lead + y0);
    int v_shift = (int)(v_subpixel);
    v_subpixel -= v_shift;

    memset(d->tmp, 0, d->tmp_width * d->tmp_height);
    stbtt_MakeCodepointBitmapSubpixel(&d->font->info, d->tmp,
                              d->tmp_width, d->tmp_height, d->tmp_width,
                              d->font->scale, d->font->scale, h_subpixel, v_subpixel,c);
    
    float width = (x1 - x0) * d->font->scale + 1;
    float height = (y1 - y0) * d->font->scale + 1;
    unsigned char* dOff = d->out + h_shift * 4;
    int y_max = (int)height + 1;
    if (d->font->max_height < y_max)
        y_max = d->font->max_width;
    int x_max = (int)width + 1;
    if (d->font->max_width < x_max)
        x_max = d->font->max_height;

    for (int y = 0; y < y_max; y++) {
        int dy = y + v_shift;
        unsigned char* dRowStart = dOff + (dy * d->out_width * 4);
        unsigned char* sRowStart = d->tmp + (y * d->font->max_width);
        for (int x = 0; x < x_max; x++) {
            unsigned char* dst = dRowStart + (4 * x + 3);
            if (dst >= d->out && dst < d->out_end) // Range check ... JUST in case
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

static void s_plot_one_char(const char c, nate_PrintOneChar* d)
{
    int x0 = 0, x1 = 0, y0 = 0, y1 = 0, adv = 0, bear = 0;
    stbtt_GetCodepointHMetrics(&d->font->info, c, &adv, &bear);
    if (bear < 0 && d->h_offset < -bear)
        bear = - bear;
    d->h_offset += bear;
    s_plot_one_char_inner(c, d);   
    d->h_offset += adv;
}

static SDL_Surface* s_text_render(const char* text, nate_ByteBuffer* buffer, nate_FontExt* font)
{
    if (!font) return NULL;
    
    const char* t = text;
    size_t final_width = s_final_text_width_unscaled(text, font);
    if (!final_width) {
        fprintf(stderr, "Failed to get width of string \"%s\"", text);
        return NULL;
    }
    // printf("Final width: %f\n", final_width * s_font.scale);
    final_width = (int)(final_width * font->scale) + 1;

    if (!nate_ByteBuffer_Alloc(buffer, font->max_height * final_width * 4))
        return NULL;
    buffer->size = font->max_height * final_width * 4;
    memset(buffer->data, 0, buffer->size);

    unsigned char* tmpbuff = (unsigned char*)malloc(font->max_width * font->max_height);
    if (!tmpbuff) {
        fprintf(stderr, "Failed to allocate %i bytes temporarly\n",
                font->max_width * font->max_height);
        return NULL;
    }

    nate_PrintOneChar printOneChar = {
        .tmp = tmpbuff,
        .tmp_width = font->max_width,
        .tmp_height = font->max_height,
        .out = (unsigned char*)buffer->data,
        .out_end = (unsigned char*)buffer->data + buffer->size,
        .out_width = final_width,
        .h_offset = 0,
        .font = font,
    };
    t = text;
    while(*t != '\0') {
        s_plot_one_char(*t, &printOneChar);
        t++;
    }
    free(tmpbuff);

    return SDL_CreateSurfaceFrom(final_width, font->max_height,
                                 SDL_PIXELFORMAT_RGBA32, buffer->data, final_width * 4);
}

SDL_Surface* nate_Text_Render(const char* text, nate_ByteBuffer* buffer)
{
    if (!text || !buffer)
        return NULL;
    else
        return s_text_render(text, buffer, &s_font_default);
}

SDL_Surface* nate_Text_Render_Sized(const char* text, nate_ByteBuffer* buffer, float size)
{
    if (!text || !buffer)
        return NULL;
    
    struct nate_FontExt nu = s_font_default;
    s_set_font_size(size, &nu);
    return s_text_render(text, buffer, &nu);
}
