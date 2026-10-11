#include "ship.h"
#include <string.h>
#include <stdlib.h>

int get_ship_size(char type) {
    switch (type) {
        case 'S': return 5; // Shinano
        case 'Y': return 4; // Yamato
        case 'B': return 3; // Belfast
        case 'L': return 2; // Laffey
        case 'A': return 1; // Albacore
        default:  return 0;
    }
}

const char *get_ship_name(char type) {
    switch (type) {
        case 'S': return "Shinano";
        case 'Y': return "Yamato";
        case 'B': return "Belfast";
        case 'L': return "Laffey";
        case 'A': return "Albacore";
        default:  return "Unknown";
    }
}

// Compute total ships per player based on board size
int calc_ships_per_player(int rows, int cols) {
    int total_cells = rows * cols;
    int nr_S = total_cells / 70;
    int nr_Y = total_cells / 55;
    int nr_B = total_cells / 40;
    int nr_L = total_cells / 30;
    int nr_A = total_cells / 20;

    int total = nr_S + nr_Y + nr_B + nr_L + nr_A;
    // Guarantee at least 1 ship if board is very small
    return (total > 0) ? total : 1;
}

int can_place_ship(const Match *game, const Player *player,
                   char dir, int length, int head_x, int head_y) {
    if (head_x < 0 || head_x >= game->rows || head_y < 0 || head_y >= game->cols) {
        return 0;
    }

    if (dir == 'H') {
        // Horizontal: extends to the right
        if (head_y + length - 1 >= game->cols) return 0;

        for (int j = 0; j < length; j++) {
            if (player->board[head_x][head_y + j] != CELL_WATER) return 0;
        }
    } else if (dir == 'V') {
        // Vertical: extends upwards
        if (head_x - length + 1 < 0) return 0;

        for (int i = 0; i < length; i++) {
            if (player->board[head_x - i][head_y] != CELL_WATER) return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

void init_ship(Ship *ship, char type, char dir, int head_x, int head_y) {
    ship->type = type;
    strncpy(ship->name, get_ship_name(type), sizeof(ship->name) - 1);
    ship->name[sizeof(ship->name) - 1] = '\0';
    ship->dir = dir;
    ship->head_x = head_x;
    ship->head_y = head_y;
    ship->length = get_ship_size(type);
    ship->hp = ship->length;
    ship->is_dead = 0;
}

void place_ship_on_board(Player *player, int ship_idx) {
    Ship *s = &player->ships[ship_idx];
    int id = ship_idx + 1; // 1-based unique ID

    if (s->dir == 'H') {
        for (int j = 0; j < s->length; j++) {
            player->board[s->head_x][s->head_y + j] = id;
        }
    } else {
        for (int i = 0; i < s->length; i++) {
            player->board[s->head_x - i][s->head_y] = id;
        }
    }
}

void mark_sunk_on_board(Player *player, int ship_idx) {
    Ship *s = &player->ships[ship_idx];
    if (s->dir == 'H') {
        for (int j = 0; j < s->length; j++) {
            player->board[s->head_x][s->head_y + j] = CELL_SUNK;
        }
    } else {
        for (int i = 0; i < s->length; i++) {
            player->board[s->head_x - i][s->head_y] = CELL_SUNK;
        }
    }
}

// Randomly generate valid positions for the whole fleet
void auto_place_all_ships(const Match *game, Player *player) {
    // Generate fleet composition according to size
    int total_cells = game->rows * game->cols;
    int counts[5] = {
        total_cells / 70, // S
        total_cells / 55, // Y
        total_cells / 40, // B
        total_cells / 30, // L
        total_cells / 20  // A
    };
    char types[5] = {'S', 'Y', 'B', 'L', 'A'};

    int ship_idx = 0;
    for (int t = 0; t < 5 && ship_idx < player->ship_count; t++) {
        int count = counts[t];
        for (int k = 0; k < count && ship_idx < player->ship_count; k++) {
            char type = types[t];
            int length = get_ship_size(type);

            int placed = 0;
            int tries = 0;
            while (!placed && tries < 2000) {
                tries++;
                char dir = (rand() % 2 == 0) ? 'H' : 'V';
                int r = rand() % game->rows;
                int c = rand() % game->cols;

                if (can_place_ship(game, player, dir, length, r, c)) {
                    init_ship(&player->ships[ship_idx], type, dir, r, c);
                    place_ship_on_board(player, ship_idx);
                    ship_idx++;
                    placed = 1;
                }
            }
        }
    }

    // Fill any remaining ship slots with small boats (A)
    while (ship_idx < player->ship_count) {
        int r = rand() % game->rows;
        int c = rand() % game->cols;
        if (can_place_ship(game, player, 'H', 1, r, c)) {
            init_ship(&player->ships[ship_idx], 'A', 'H', r, c);
            place_ship_on_board(player, ship_idx);
            ship_idx++;
        }
    }
}
