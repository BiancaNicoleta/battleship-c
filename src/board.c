#include "board.h"
#include <stdio.h>
#include <stdlib.h>

// Allocate 2D matrix
int **alloc_board(int rows, int cols) {
    if (rows <= 0 || cols <= 0) return NULL;

    int **board = (int **)malloc(rows * sizeof(int *));
    if (!board) return NULL;

    for (int i = 0; i < rows; i++) {
        board[i] = (int *)malloc(cols * sizeof(int));
        if (!board[i]) {
            for (int j = 0; j < i; j++) free(board[j]);
            free(board);
            return NULL;
        }
    }
    return board;
}

// Free 2D matrix
void free_board(int rows, int **board) {
    if (!board) return;
    for (int i = 0; i < rows; i++) {
        free(board[i]);
    }
    free(board);
}

// Fill matrix with a given value
void clear_board(int rows, int cols, int **board, int val) {
    if (!board) return;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i][j] = val;
        }
    }
}

// Check if (r, c) is inside the board
int is_valid_coord(const Match *game, int r, int c) {
    if (!game) return 0;
    return (r >= 0 && r < game->rows && c >= 0 && c < game->cols);
}

void clear_screen(void) {
    printf("\033[H\033[J");
    fflush(stdout);
}

void print_banner(void) {
    printf("\n");
    printf("  ================================================\n");
    printf("                     BATTLESHIP                   \n");
    printf("  ================================================\n\n");
}

int count_alive_ships(const Player *player) {
    int alive = 0;
    for (int i = 0; i < player->ship_count; i++) {
        if (player->ships[i].length > 0 && !player->ships[i].is_dead) {
            alive++;
        }
    }
    return alive;
}

static void print_cell(int val, const Player *owner, int show_ships) {
    if (val == CELL_WATER) {
        printf(COLOR_CYAN " ~ " COLOR_RESET);
    } else if (val == CELL_MISS) {
        printf(COLOR_GRAY " o " COLOR_RESET);
    } else if (val == CELL_HIT) {
        printf(COLOR_RED " X " COLOR_RESET);
    } else if (val == CELL_SUNK) {
        printf(COLOR_YELLOW " # " COLOR_RESET);
    } else if (val > 0) {
        if (show_ships && owner && val <= owner->ship_count) {
            char type = owner->ships[val - 1].type;
            printf(COLOR_GREEN " %c " COLOR_RESET, type);
        } else {
            // Hidden in fog of war
            printf(COLOR_CYAN " ~ " COLOR_RESET);
        }
    } else {
        printf(" ? ");
    }
}

// Print both boards side-by-side with exact character alignment
void print_game_view(const Match *game, int current_turn_player_id) {
    const Player *me = (current_turn_player_id == 1) ? &game->p1 : &game->p2;
    const Player *enemy = (current_turn_player_id == 1) ? &game->p2 : &game->p1;

    int me_alive = count_alive_ships(me);
    int enemy_alive = count_alive_ships(enemy);

    printf("    -- %s (YOUR FLEET) --                  -- ENEMY RADAR --\n", me->name);

    // Column numbers
    printf(COLOR_DIM "   ");
    for (int j = 0; j < game->cols; j++) {
        if (j < 9) printf(" %d ", j + 1);
        else printf("%2d ", j + 1);
    }
    printf("  |     ");
    for (int j = 0; j < game->cols; j++) {
        if (j < 9) printf(" %d ", j + 1);
        else printf("%2d ", j + 1);
    }
    printf("\n" COLOR_RESET);

    // Board rows
    for (int i = 0; i < game->rows; i++) {
        // Left board row
        printf(COLOR_DIM "%2d " COLOR_RESET, i + 1);
        for (int j = 0; j < game->cols; j++) {
            print_cell(me->board[i][j], me, 1);
        }

        // Divider
        printf("  |  ");

        // Right board row
        printf(COLOR_DIM "%2d " COLOR_RESET, i + 1);
        for (int j = 0; j < game->cols; j++) {
            print_cell(enemy->board[i][j], enemy, 0);
        }
        printf("\n");
    }

    // Legend & stats
    printf("\n  Legend: " COLOR_CYAN "~ Water" COLOR_RESET
           "  " COLOR_GREEN "S/Y/B/L/A Ship" COLOR_RESET
           "  " COLOR_RED "X Hit" COLOR_RESET
           "  " COLOR_GRAY "o Miss" COLOR_RESET
           "  " COLOR_YELLOW "# Sunk" COLOR_RESET "\n");

    printf("  Status: %s: %d/%d alive  |  %s: %d/%d remaining\n\n",
           me->name, me_alive, me->ship_count,
           enemy->name, enemy_alive, enemy->ship_count);
}

void print_single_board(const Match *game, const Player *player, const char *title) {
    if (title) printf("  %s:\n", title);

    printf(COLOR_DIM "   ");
    for (int j = 0; j < game->cols; j++) {
        if (j < 9) printf(" %d ", j + 1);
        else printf("%2d ", j + 1);
    }
    printf("\n" COLOR_RESET);

    for (int i = 0; i < game->rows; i++) {
        printf(COLOR_DIM "%2d " COLOR_RESET, i + 1);
        for (int j = 0; j < game->cols; j++) {
            print_cell(player->board[i][j], player, 1);
        }
        printf("\n");
    }
    printf("\n");
}
