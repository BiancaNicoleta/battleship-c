# Battleship in C

A terminal-based Battleship game written in C99. Features a clean dual-board display with ANSI colors, a single-player mode against a computer AI, local two-player mode, and a custom **headshot rule**.

---

## Rules & How to Play

The game is played on a grid (default 10x10) where both players secretly place their fleet:

- **Turn-based combat**: Players take turns entering coordinates `(row col)` to attack the opponent's grid.
- **Hits and Misses**:
  - If a shot hits open water, it is marked as a miss (`o`).
  - If a shot hits a ship segment, it is marked as a hit (`X`) and reduces the ship's HP by 1.
- **The Headshot Rule**:
  - Each ship has a "head" (its starting placement coordinate).
  - If you hit the head directly, the entire ship is destroyed instantly, regardless of its size.
- **Winning**: The first player to sink all enemy ships wins the match.

---

## Ships

The game includes 5 types of historical warships:

| Symbol | Name | Class | Length |
| :---: | :--- | :--- | :---: |
| **S** | **Shinano** | Aircraft Carrier | **5** |
| **Y** | **Yamato** | Battleship | **4** |
| **B** | **Belfast** | Cruiser | **3** |
| **L** | **Laffey** | Destroyer | **2** |
| **A** | **Albacore** | Submarine | **1** |

On a standard 10x10 board, each player commands 12 ships in total (1 Shinano, 1 Yamato, 2 Belfasts, 3 Laffeys, and 5 Albacores).

---

## Features

- **Single Player (vs AI)**: Play against a computer bot that searches for ships and targets adjacent cells once it lands a hit.
- **Two Players (Pass & Play)**: Local multiplayer with screen-clearing turns to keep secret fleet positions hidden.
- **Fleet Placement**: Choose between fast automatic random placement or manual coordinate placement.
- **Side-by-side Terminal Display**: Shows your own fleet and the enemy radar (with fog of war) side by side.

---

## Board Display & Legend

```text
    -- Player 1 (YOUR FLEET) --                  -- ENEMY RADAR --
    1  2  3  4  5  6  7  8  9 10   |      1  2  3  4  5  6  7  8  9 10 
 1  ~  ~  A  ~  ~  ~  ~  ~  ~  A   |   1  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~ 
 2  ~  ~  B  ~  ~  ~  ~  S  ~  ~   |   2  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~ 
 3  ~  B  B  ~  ~  ~  ~  S  ~  ~   |   3  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~ 
 4  ~  B  B  ~  ~  ~  A  S  ~  ~   |   4  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~ 
 5  ~  B  ~  ~  ~  L  L  S  ~  ~   |   5  ~  ~  ~  ~  ~  ~  ~  ~  ~  ~ 
```

| Symbol | Color | Meaning |
| :---: | :---: | :--- |
| `~` | Cyan | Open water |
| `S / Y / B / L / A` | Green | Your ships |
| `X` | Red | Hit on a ship |
| `o` | Gray | Missed shot |
| `#` | Yellow | Sunk ship |

---

## Project Structure

```text
battleship-c/
??? src/
?   ??? main.c       # Game loop and menus
?   ??? board.c/.h   # Grid allocation and terminal display
?   ??? ship.c/.h    # Placement validation and fleet generator
?   ??? game.c/.h    # Attack logic, headshot rule, and AI bot
?   ??? structs.h    # Shared data types and constants
??? Makefile         # Build configuration
??? README.md
```

---

## Build & Run

Compile the game using `make`:
```bash
make
```

Run the executable:
```bash
./battleships
```

Clean build files:
```bash
make clean
```
