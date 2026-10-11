#ifndef GAME_H
#define GAME_H

#include "structs.h"

int init_match(Match *game, int rows, int cols,
               const char *name1, const char *name2, int p2_is_bot);
void free_match(Match *game);

AttackResult execute_attack(Match *game, int attacker_id, int x, int y,
                           const Ship **out_ship, int *out_end_x, int *out_end_y,
                           int *out_was_headshot);

int are_all_ships_sunk(const Player *player);

// Simple AI logic: hunts adjacent cells if hit, otherwise random
void bot_choose_target(const Match *game, int bot_player_id, int *out_r, int *out_c);

#endif // GAME_H
