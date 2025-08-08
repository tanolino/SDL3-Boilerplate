#include "game.h"

#define s_card_verts_len 4
static SDL_Vertex s_card_verts[s_card_verts_len] = { 0 };
static int s_card_index[] = {0, 1, 2, 2, 3, 0};

static SDL_Texture* s_card_texture = NULL;

static void s_draw_card()
{
    SDL_RenderGeometry(nate_renderer, s_card_texture,
                       s_card_verts, s_card_verts_len,
                       s_card_index, sizeof(s_card_index) / sizeof(int));
    SDL_SetRenderDrawColor(nate_renderer, 10, 10, 10, 0);
}

static void s_configure_card(float pos_x, float pos_y, float rotat)
{
    // Position values
    float xComp = 200.f * (rotat - 0.5f);
    float xL = pos_x + xComp;
    float xR = pos_x - xComp;
    s_card_verts[0].position.x = xL;
    s_card_verts[1].position.x = xR;
    s_card_verts[2].position.x = xR;
    s_card_verts[3].position.x = xL;
    
    float yU = pos_y + 160.f;
    float yL = pos_y - 160.f;
    float rearOff = (rotat - 0.5f) * 2;
    rearOff *= rearOff;
    rearOff = 1.f - rearOff;
    rearOff *= 30.f;
    s_card_verts[0].position.y = yU;
    s_card_verts[1].position.y = yU - rearOff;
    s_card_verts[2].position.y = yL + rearOff;
    s_card_verts[3].position.y = yL;

    // Texture coordinated
    if (rotat > 0.5f) {
        s_card_verts[0].tex_coord.x = 0.5f;
        s_card_verts[1].tex_coord.x = 1.0f;
        s_card_verts[2].tex_coord.x = 1.0f;
        s_card_verts[3].tex_coord.x = 0.5f;
    }
    else {
        s_card_verts[0].tex_coord.x = 0.0f;
        s_card_verts[1].tex_coord.x = 0.5f;
        s_card_verts[2].tex_coord.x = 0.5f;
        s_card_verts[3].tex_coord.x = 0.0f;
    }
    s_card_verts[0].tex_coord.y = 0.f;
    s_card_verts[1].tex_coord.y = 0.f;
    s_card_verts[2].tex_coord.y = 1.f;
    s_card_verts[3].tex_coord.y = 1.f;
}

SDL_AppResult game_Init()
{
    s_configure_card(200.f, 200.f, 0.f);
    for (int i = 0; i < s_card_verts_len; i++) {
        s_card_verts[i].color.r = 0xFF;
        s_card_verts[i].color.g = 0xFF;
        s_card_verts[i].color.b = 0xFF;
        s_card_verts[i].color.a = 0xFF;
    }

    nate_ByteBuffer tmp_text = nate_ByteBuffer0;
    nate_ByteBuffer tmp_card = nate_ByteBuffer0;
    nate_MemoryOf3rd mem3rd = nate_MemoryOf3rd0;
    SDL_Surface* surface_text = NULL;
    SDL_Surface* surface_card = NULL;
    do {
        surface_text = nate_Text_Render_Sized("Le card", &tmp_text, 120.f);
        if (!surface_text)
            break;
        SDL_FlipSurface(surface_text, SDL_FLIP_VERTICAL);
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
#define max_sum 1000000000ull

extern SDL_AppResult game_Iterate(Uint64 ticks)
{
    if (s_mouse_over_card()) {
        card_animation += ticks;
        if (card_animation > max_sum)
            card_animation = max_sum;
    } else if (card_animation < ticks) {
        card_animation = 0ull;
    } else {
        card_animation -= ticks;
    }
    
    float progress = (float)card_animation / (float)max_sum;
    s_configure_card(200.f, 200.f, progress);
    s_draw_card();
    return SDL_APP_CONTINUE;
}

extern void game_Quit() {}
