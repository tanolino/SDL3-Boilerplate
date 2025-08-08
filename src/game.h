#ifndef _NATE_GAME
#define _NATE_GAME

#include "main.h"

// Init state
extern SDL_AppResult game_Init();

// Reacts to events
extern SDL_AppResult game_Event(const SDL_Event* ev);

// Process timed events and Draw the Screen
extern SDL_AppResult game_Iterate(Uint64 ticks);

// Clean up
extern void game_Quit();

#endif // _NATE_GAME
