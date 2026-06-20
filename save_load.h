#pragma once
#include "game_state.h"
#include <stdbool.h>

bool save_game(const GameState* gs, int dead_player_idx);
bool load_game(GameState* gs, int* dead_player_idx);
bool save_exists(void);
void delete_save(void);
