#ifndef _NATE_GAME
#define _NATE_GAME

// #include <SDL3/SDL.h>

#define CARD_WIDTH 199
#define CARD_HEIGHT 301
typedef struct game_CardTag {
    float x;
    float y;
    float scale;
    float rotation; // 0.0 to 0.5 is front side
    float highlight;
} game_Card;
#define game_Card0 (game_Card){ \
    .x = 0.f, .y = 0.f, \
    .scale = 1.f, .rotation = 0.f, \
    .highlight = 0.f \
}

#endif // _NATE_GAME
