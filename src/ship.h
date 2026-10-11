#ifndef SHIP_H
#define SHIP_H

#include "structs.h"

int get_ship_size(char type);
const char *get_ship_name(char type);
int calc_ships_per_player(int rows, int cols);

// Check if a ship fits at (head_x, head_y)
int can_place_ship(const Match *game, const Player *player,
                   char dir, int length, int head_x, int head_y);

void init_ship(Ship *ship, char type, char dir, int head_x, int head_y);
void place_ship_on_board(Player *player, int ship_idx);
void mark_sunk_on_board(Player *player, int ship_idx);

// Randomly place all ships for a player
void auto_place_all_ships(const Match *game, Player *player);

#endif // SHIP_H
