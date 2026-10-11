#ifndef BOARD_H
#define BOARD_H

#include "structs.h"

// Terminal colors
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_DIM     "\033[2m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_GRAY    "\033[90m"

// Board allocation and cleanup
int **alloc_board(int rows, int cols);
void free_board(int rows, int **board);
void clear_board(int rows, int cols, int **board, int val);

// Coordinate check
int is_valid_coord(const Match *game, int r, int c);

// Visual display
void clear_screen(void);
void print_banner(void);
int count_alive_ships(const Player *player);
void print_game_view(const Match *game, int current_turn_player_id);
void print_single_board(const Match *game, const Player *player, const char *title);

#endif // BOARD_H
