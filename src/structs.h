#ifndef STRUCTS_H
#define STRUCTS_H

// Board cell values
#define CELL_WATER       0
#define CELL_MISS       -1
#define CELL_HIT        -2
#define CELL_SUNK       -3

// Attack results
typedef enum {
    ATTACK_OUT_OF_BOUNDS,
    ATTACK_ALREADY_HIT,
    ATTACK_MISS,
    ATTACK_HIT,
    ATTACK_SUNK,
    ATTACK_WIN
} AttackResult;

// Ship structure
typedef struct {
    char type;          // 'S', 'Y', 'B', 'L', 'A'
    char name[16];      // "Shinano", "Yamato", etc.
    char dir;           // 'H' (horizontal) or 'V' (vertical)
    int head_x, head_y; // 0-indexed head coordinates
    int length;
    int hp;             // Remaining undamaged parts
    int is_dead;
} Ship;

// Player structure
typedef struct {
    int id;             // 1 or 2
    char name[32];      // "Player 1", "Player 2", "Bot"
    int **board;        // Dynamic 2D matrix
    Ship *ships;        // Array of ships
    int ship_count;
    int is_bot;         // 1 if AI, 0 if human
} Player;

// Match structure
typedef struct {
    int rows, cols;
    int ships_per_player;
    int total_ships;
    Player p1;
    Player p2;
} Match;

#endif // STRUCTS_H
