#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "structs.h"
#include "board.h"
#include "ship.h"
#include "game.h"

// Helper to pause until user presses Enter
static void wait_for_enter(const char *msg) {
    if (msg) printf("%s", msg);
    else printf(COLOR_DIM "Press ENTER to continue..." COLOR_RESET);
    fflush(stdout);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Deploy fleet either manually or randomly
static void deploy_fleet(Match *game, Player *player) {
    if (player->is_bot) {
        auto_place_all_ships(game, player);
        return;
    }

    clear_screen();
    print_banner();
    printf("  Deploying fleet for %s (%d ships total):\n", player->name, player->ship_count);
    printf("  1. Automatically place ships randomly\n");
    printf("  2. Manually enter coordinates for each ship\n");
    printf("  Choose option (1 or 2): ");
    fflush(stdout);

    int choice = 1;
    if (scanf("%d", &choice) != 1) choice = 1;

    if (choice == 1) {
        auto_place_all_ships(game, player);
        clear_screen();
        print_banner();
        printf("  %s fleet deployed successfully!\n\n", player->name);
        print_single_board(game, player, "Your Deployed Fleet");
        wait_for_enter(NULL);
        return;
    }

    // Manual placement
    char types[] = {'S', 'Y', 'B', 'B', 'L', 'L', 'L', 'A', 'A', 'A', 'A', 'A'};
    int available = sizeof(types) / sizeof(types[0]);

    for (int idx = 0; idx < player->ship_count; idx++) {
        char type = (idx < available) ? types[idx] : 'A';
        int length = get_ship_size(type);
        const char *name = get_ship_name(type);

        int placed = 0;
        while (!placed) {
            clear_screen();
            print_banner();
            printf("  %s: Placing ship %d of %d\n\n", player->name, idx + 1, player->ship_count);
            print_single_board(game, player, "Current Board");

            printf("  Deploying %s (Length: %d)\n", name, length);
            printf("  Note: H extends right, V extends up.\n");
            printf("  Enter direction and coordinates [H/V row col] (e.g. H 2 3): ");
            fflush(stdout);

            char dir;
            int r, c;
            if (scanf(" %c %d %d", &dir, &r, &c) != 3) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue;
            }

            if (dir == 'h') dir = 'H';
            if (dir == 'v') dir = 'V';

            int head_x = r - 1;
            int head_y = c - 1;

            if (can_place_ship(game, player, dir, length, head_x, head_y)) {
                init_ship(&player->ships[idx], type, dir, head_x, head_y);
                place_ship_on_board(player, idx);
                placed = 1;
            } else {
                printf(COLOR_RED "\n  Invalid position! Ship collides or goes out of bounds.\n" COLOR_RESET);
                wait_for_enter(NULL);
            }
        }
    }

    clear_screen();
    print_banner();
    printf("  %s fleet deployed successfully!\n\n", player->name);
    print_single_board(game, player, "Your Deployed Fleet");
    wait_for_enter(NULL);
}

// Interactive game runner
static void play_game(void) {
    clear_screen();
    print_banner();

    printf("  Choose Game Mode:\n");
    printf("  1. Single Player (You vs Computer AI)\n");
    printf("  2. Two Players (Pass & Play)\n");
    printf("  Select mode (1 or 2): ");
    fflush(stdout);

    int mode = 1;
    if (scanf("%d", &mode) != 1) mode = 1;

    int is_vs_bot = (mode == 1);

    printf("\n  Enter board size [rows cols] (default 10 10): ");
    fflush(stdout);

    int rows = 10, cols = 10;
    if (scanf("%d %d", &rows, &cols) != 2 || rows < 5 || cols < 5) {
        rows = 10;
        cols = 10;
    }

    Match game;
    const char *p1_name = "Player 1";
    const char *p2_name = is_vs_bot ? "Computer" : "Player 2";

    if (!init_match(&game, rows, cols, p1_name, p2_name, is_vs_bot)) {
        printf("Error: Could not allocate match memory.\n");
        return;
    }

    // Deploy fleets
    deploy_fleet(&game, &game.p1);
    deploy_fleet(&game, &game.p2);

    // Battle loop
    int turn = 1;
    char last_event[256] = "Match started. Make your first attack.";
    int game_over = 0;

    while (!game_over) {
        Player *current = (turn == 1) ? &game.p1 : &game.p2;

        if (!current->is_bot) {
            clear_screen();
            print_banner();
            printf("  Last event: %s\n\n", last_event);
            print_game_view(&game, turn);

            printf("  %s, enter attack coordinates (row col) [or 0 0 to quit]: ", current->name);
            fflush(stdout);

            int target_r, target_c;
            if (scanf("%d %d", &target_r, &target_c) != 2) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
                continue;
            }

            if (target_r == 0 && target_c == 0) {
                printf("\n  Game exited.\n");
                break;
            }

            int x = target_r - 1;
            int y = target_c - 1;
            const Ship *sunk_ship = NULL;
            int end_x = 0, end_y = 0;
            int was_headshot = 0;

            AttackResult res = execute_attack(&game, turn, x, y, &sunk_ship, &end_x, &end_y, &was_headshot);

            if (res == ATTACK_OUT_OF_BOUNDS) {
                snprintf(last_event, sizeof(last_event),
                         COLOR_RED "Coordinates (%d, %d) are out of bounds!" COLOR_RESET,
                         target_r, target_c);
                continue;
            } else if (res == ATTACK_ALREADY_HIT) {
                snprintf(last_event, sizeof(last_event),
                         COLOR_YELLOW "Coordinates (%d, %d) were already attacked!" COLOR_RESET,
                         target_r, target_c);
                continue;
            } else if (res == ATTACK_MISS) {
                snprintf(last_event, sizeof(last_event),
                         "%s missed at (%d, %d).", current->name, target_r, target_c);
            } else if (res == ATTACK_HIT && sunk_ship) {
                snprintf(last_event, sizeof(last_event),
                         COLOR_GREEN "[Hit] %s hit a %s at (%d, %d)." COLOR_RESET,
                         current->name, sunk_ship->name, target_r, target_c);
            } else if ((res == ATTACK_SUNK || res == ATTACK_WIN) && sunk_ship) {
                if (was_headshot) {
                    snprintf(last_event, sizeof(last_event),
                             COLOR_YELLOW "[Critical Hit] Headshot! %s destroyed %s instantly!" COLOR_RESET,
                             current->name, sunk_ship->name);
                } else {
                    snprintf(last_event, sizeof(last_event),
                             COLOR_YELLOW "[Sunk] %s sunk %s placed from (%d, %d) to (%d, %d)." COLOR_RESET,
                             current->name, sunk_ship->name, sunk_ship->head_x + 1, sunk_ship->head_y + 1,
                             end_x + 1, end_y + 1);
                }

                if (res == ATTACK_WIN) {
                    clear_screen();
                    print_banner();
                    print_game_view(&game, turn);
                    printf("\n  ================================================\n");
                    printf("     GAME OVER: %s won the game!\n", current->name);
                    printf("  ================================================\n\n");
                    game_over = 1;
                    wait_for_enter(NULL);
                    break;
                }
            }

            if (!is_vs_bot) {
                printf("\n  %s\n", last_event);
                wait_for_enter(COLOR_DIM "Press ENTER to pass to next player..." COLOR_RESET);
            }

        } else {
            // Bot's turn
            int bot_x, bot_y;
            bot_choose_target(&game, turn, &bot_x, &bot_y);

            const Ship *sunk_ship = NULL;
            int end_x = 0, end_y = 0;
            int was_headshot = 0;

            AttackResult res = execute_attack(&game, turn, bot_x, bot_y, &sunk_ship, &end_x, &end_y, &was_headshot);

            if (res == ATTACK_MISS) {
                snprintf(last_event, sizeof(last_event),
                         "Computer shot at (%d, %d) and missed.", bot_x + 1, bot_y + 1);
            } else if (res == ATTACK_HIT && sunk_ship) {
                snprintf(last_event, sizeof(last_event),
                         COLOR_RED "[Enemy Hit] Computer hit your %s at (%d, %d)!" COLOR_RESET,
                         sunk_ship->name, bot_x + 1, bot_y + 1);
            } else if ((res == ATTACK_SUNK || res == ATTACK_WIN) && sunk_ship) {
                if (was_headshot) {
                    snprintf(last_event, sizeof(last_event),
                             COLOR_RED "[Enemy Critical] Computer sunk your %s with a headshot!" COLOR_RESET,
                             sunk_ship->name);
                } else {
                    snprintf(last_event, sizeof(last_event),
                             COLOR_RED "[Enemy Sunk] Computer sunk your %s!" COLOR_RESET,
                             sunk_ship->name);
                }

                if (res == ATTACK_WIN) {
                    clear_screen();
                    print_banner();
                    print_game_view(&game, 1);
                    printf("\n  ================================================\n");
                    printf("     GAME OVER: Computer won the match.\n");
                    printf("  ================================================\n\n");
                    game_over = 1;
                    wait_for_enter(NULL);
                    break;
                }
            }
        }

        turn = 3 - turn;
    }

    free_match(&game);
}

// Fallback batch runner if input is redirected via pipe
static void run_batch_pipeline(void) {
    int matches = 0;
    if (scanf("%d", &matches) != 1) return;

    while (matches > 0) {
        int r, c;
        if (scanf("%d %d", &r, &c) != 2) break;

        Match game;
        if (!init_match(&game, r, c, "Player 1", "Player 2", 0)) break;

        auto_place_all_ships(&game, &game.p1);
        auto_place_all_ships(&game, &game.p2);

        int ax, ay;
        int turn = 1;
        while (scanf("%d %d", &ax, &ay) == 2) {
            const Ship *s = NULL;
            int ex, ey, hs;
            AttackResult res = execute_attack(&game, turn, ax - 1, ay - 1, &s, &ex, &ey, &hs);
            if (res == ATTACK_WIN) break;
            turn = 3 - turn;
        }

        free_match(&game);
        matches--;
    }
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    int is_interactive = isatty(STDIN_FILENO);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--batch") == 0) is_interactive = 0;
        if (strcmp(argv[i], "-i") == 0) is_interactive = 1;
    }

    if (is_interactive) {
        play_game();
    } else {
        run_batch_pipeline();
    }

    return 0;
}
