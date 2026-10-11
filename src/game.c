#include "game.h"
#include "board.h"
#include "ship.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_match(Match *game, int rows, int cols,
               const char *name1, const char *name2, int p2_is_bot) {
    game->rows = rows;
    game->cols = cols;
    game->ships_per_player = calc_ships_per_player(rows, cols);
    game->total_ships = game->ships_per_player * 2;

    // Player 1
    game->p1.id = 1;
    strncpy(game->p1.name, name1, sizeof(game->p1.name) - 1);
    game->p1.name[sizeof(game->p1.name) - 1] = '\0';
    game->p1.is_bot = 0;
    game->p1.board = alloc_board(rows, cols);
    if (!game->p1.board) return 0;
    clear_board(rows, cols, game->p1.board, CELL_WATER);

    // Player 2
    game->p2.id = 2;
    strncpy(game->p2.name, name2, sizeof(game->p2.name) - 1);
    game->p2.name[sizeof(game->p2.name) - 1] = '\0';
    game->p2.is_bot = p2_is_bot;
    game->p2.board = alloc_board(rows, cols);
    if (!game->p2.board) {
        free_board(rows, game->p1.board);
        return 0;
    }
    clear_board(rows, cols, game->p2.board, CELL_WATER);

    // Ships
    game->p1.ship_count = game->ships_per_player;
    game->p1.ships = (Ship *)calloc(game->ships_per_player, sizeof(Ship));
    if (!game->p1.ships) {
        free_board(rows, game->p1.board);
        free_board(rows, game->p2.board);
        return 0;
    }

    game->p2.ship_count = game->ships_per_player;
    game->p2.ships = (Ship *)calloc(game->ships_per_player, sizeof(Ship));
    if (!game->p2.ships) {
        free(game->p1.ships);
        free_board(rows, game->p1.board);
        free_board(rows, game->p2.board);
        return 0;
    }

    return 1;
}

void free_match(Match *game) {
    if (!game) return;
    free_board(game->rows, game->p1.board);
    free_board(game->rows, game->p2.board);
    free(game->p1.ships);
    free(game->p2.ships);
    game->p1.board = NULL;
    game->p2.board = NULL;
    game->p1.ships = NULL;
    game->p2.ships = NULL;
}

int are_all_ships_sunk(const Player *player) {
    for (int i = 0; i < player->ship_count; i++) {
        if (player->ships[i].length > 0 && !player->ships[i].is_dead) {
            return 0;
        }
    }
    return 1;
}

AttackResult execute_attack(Match *game, int attacker_id, int x, int y,
                           const Ship **out_ship, int *out_end_x, int *out_end_y,
                           int *out_was_headshot) {
    if (out_was_headshot) *out_was_headshot = 0;

    if (!is_valid_coord(game, x, y)) {
        return ATTACK_OUT_OF_BOUNDS;
    }

    Player *defender = (attacker_id == 1) ? &game->p2 : &game->p1;
    int cell = defender->board[x][y];

    // Already targeted
    if (cell == CELL_MISS || cell == CELL_HIT || cell == CELL_SUNK) {
        return ATTACK_ALREADY_HIT;
    }

    // Missed
    if (cell == CELL_WATER) {
        defender->board[x][y] = CELL_MISS;
        return ATTACK_MISS;
    }

    // Hit a ship! (cell > 0 stores ship_idx + 1)
    int ship_idx = cell - 1;
    Ship *target = &defender->ships[ship_idx];
    if (out_ship) *out_ship = target;

    defender->board[x][y] = CELL_HIT;

    // Check headshot rule (critical hit)
    if (x == target->head_x && y == target->head_y) {
        target->hp = 0; // Immediate destruction
        if (out_was_headshot) *out_was_headshot = 1;
    } else {
        target->hp--;
    }

    if (target->hp > 0) {
        return ATTACK_HIT;
    }

    // Ship sunk
    target->is_dead = 1;
    mark_sunk_on_board(defender, ship_idx);

    if (out_end_x && out_end_y) {
        if (target->dir == 'H') {
            *out_end_x = target->head_x;
            *out_end_y = target->head_y + target->length - 1;
        } else {
            *out_end_x = target->head_x - target->length + 1;
            *out_end_y = target->head_y;
        }
    }

    if (are_all_ships_sunk(defender)) {
        return ATTACK_WIN;
    }

    return ATTACK_SUNK;
}

// Simple bot: target adjacent cells to hits, otherwise choose random water
void bot_choose_target(const Match *game, int bot_player_id, int *out_r, int *out_c) {
    const Player *defender = (bot_player_id == 1) ? &game->p2 : &game->p1;

    // 1. Target mode: look for any damaged cell (CELL_HIT) and check its neighbors
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            if (defender->board[i][j] == CELL_HIT) {
                // Check 4 adjacent directions
                for (int d = 0; d < 4; d++) {
                    int nr = i + dr[d];
                    int nc = j + dc[d];
                    if (is_valid_coord(game, nr, nc)) {
                        int val = defender->board[nr][nc];
                        // If unattacked (water or ship)
                        if (val != CELL_MISS && val != CELL_HIT && val != CELL_SUNK) {
                            *out_r = nr;
                            *out_c = nc;
                            return;
                        }
                    }
                }
            }
        }
    }

    // 2. Hunt mode: pick random unattacked cell
    int tries = 0;
    while (tries < 5000) {
        tries++;
        int r = rand() % game->rows;
        int c = rand() % game->cols;
        int val = defender->board[r][c];
        if (val != CELL_MISS && val != CELL_HIT && val != CELL_SUNK) {
            *out_r = r;
            *out_c = c;
            return;
        }
    }

    // Fallback: linear scan
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            int val = defender->board[i][j];
            if (val != CELL_MISS && val != CELL_HIT && val != CELL_SUNK) {
                *out_r = i;
                *out_c = j;
                return;
            }
        }
    }

    *out_r = 0;
    *out_c = 0;
}
