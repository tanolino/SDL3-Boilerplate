#include "main.h"
#include "game.h"

#define s_card_verts_len 5
static SDL_Vertex s_card_verts[s_card_verts_len] = { 0 };
static float s_card_texcoords_front[s_card_verts_len * 2] = {
    0.25f, 0.5f,
    0.0f, 0.0f,
    0.5f, 0.0f,
    0.5f, 1.0f,
    0.0f, 1.0f
};
static float s_card_texcoords_back[s_card_verts_len * 2] = {
    0.75f, 0.5f,
    0.5f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.5f, 1.0f
};
static uint8_t s_card_index[] = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 1
};

static SDL_Texture* s_card_texture = NULL;

static void s_draw_card(const game_Card* card)
{
    float h_flip_val = card->rotation - 0.5f;
    float left = card->x + CARD_WIDTH * card->scale * h_flip_val;
    float right = card->x - CARD_WIDTH * card->scale * h_flip_val;
    float up = card->y - CARD_HEIGHT * card->scale / 2.f;
    float down = card->y + CARD_HEIGHT * card->scale / 2.f;
    float v_flip_val = h_flip_val * 2.f;
    v_flip_val *= v_flip_val;
    v_flip_val = 1.f - v_flip_val;
    v_flip_val *= card->scale * (CARD_HEIGHT / 10.f);
    float h_center_correction = h_flip_val * 4.f;
    if (h_center_correction < 0.f)
        h_center_correction = -h_center_correction;
    if (h_center_correction > 1.f)
        h_center_correction = 2.f - h_center_correction;
    h_center_correction *= card->scale * (CARD_WIDTH / 20.f);
    float card_verts[s_card_verts_len * 2] = {
        card->x + h_center_correction, card->y,
        left, up + v_flip_val,
        right, up - v_flip_val,
        right, down + v_flip_val,
        left, down - v_flip_val
    };
    SDL_FColor card_colors[s_card_verts_len] = {0};
    float mod = h_flip_val * 2.f;
    mod *= mod;
    mod *= card->highlight * 0.3f + 0.7f;
    for (int i = 0; i < s_card_verts_len; i++) {
        card_colors[i].r = mod;
        card_colors[i].g = mod;
        card_colors[i].b = mod;
        card_colors[i].a = 1.f;
    }
    
    bool result = SDL_RenderGeometryRaw(nate_renderer, s_card_texture,
       card_verts, sizeof(float) * 2,
       card_colors, sizeof(SDL_FColor),
       (card->rotation < 0.5f) ? s_card_texcoords_front : s_card_texcoords_back , sizeof(float) * 2,
       s_card_verts_len,
       s_card_index, sizeof(s_card_index) / sizeof(uint8_t), sizeof(uint8_t)
    );
    if (!result) {
        nate_SDL_GetError("SDL_RenderGeometryRaw()");
    }
}

SDL_AppResult game_Init()
{
    nate_ByteBuffer tmp_text = nate_ByteBuffer0;
    nate_ByteBuffer tmp_card = nate_ByteBuffer0;
    nate_MemoryOf3rd mem3rd = nate_MemoryOf3rd0;
    SDL_Surface* surface_text = NULL;
    SDL_Surface* surface_card = NULL;
    do {
        surface_text = nate_Text_Render_Sized("Le card", &tmp_text, 120.f);
        if (!surface_text)
            break;
        // SDL_FlipSurface(surface_text, SDL_FLIP_VERTICAL);
        if (!nate_Load_File("Card.png", &tmp_card))
            break;
        surface_card = nate_Load_Image(&tmp_card, &mem3rd);
        if (!surface_card)
            break;

        SDL_Rect rect = {0};
        rect.w = surface_text->w;
        rect.h = surface_text->h;
        rect.x = surface_card->w / 4.f - surface_text->w / 2.f;
        rect.y = surface_card->h / 2.f;
        SDL_BlitSurface(surface_text, NULL, surface_card, &rect);
        
        nate_LoadTextureContext ctx = {
            .result = &s_card_texture,
            .surface = surface_card,
        };
        SDL_RunOnMainThread((SDL_MainThreadCallback)nate_Load_Texture, (void*)&ctx, true);
        SDL_DestroySurface(surface_card);
    } while(false);
    if (surface_text) SDL_DestroySurface(surface_text);
    if (surface_card) SDL_DestroySurface(surface_card);
    nate_MemoryOf3rd_Free(&mem3rd);
    nate_ByteBuffer_Free(&tmp_text);
    nate_ByteBuffer_Free(&tmp_card);
    
    return SDL_APP_CONTINUE;
}

extern SDL_AppResult game_Event(const SDL_Event* ev)
{
    return SDL_APP_CONTINUE;
}

static bool s_mouse_over_card()
{
    float x = 0.f, y = 0.f;
    (void)SDL_GetMouseState(&x, &y);
    return x > 100.f && x < 300.f
        && y > 40.f && y < 360.f;
}

static uint64_t card_animation = 0ull;
#define max_sum 500000000ull

extern SDL_AppResult game_Iterate(Uint64 ticks)
{
    game_Card card = game_Card0;
    if (s_mouse_over_card()) {
        card_animation += ticks * 2;
        if (card_animation > max_sum)
            card_animation = max_sum;
        card.highlight = 1.f;
    } else if (card_animation < ticks) {
        card_animation = 0ull;
    } else {
        card_animation -= ticks;
    }
    
    float progress = (float)card_animation / (float)max_sum;
    card.scale *= 1.f + progress * 0.3f;
    card.x = 200.f;
    card.y = 200.f;
    card.rotation = progress;
    s_draw_card(&card);
    return SDL_APP_CONTINUE;
}

extern void game_Quit() {}
