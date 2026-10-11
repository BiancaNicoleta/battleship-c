# Battleship in C

A terminal-based implementation of the classic naval combat game written in **C99**. It features an aligned dual-radar interface with ANSI colors, dynamic fleet sizing, a single-player mode against a computer AI, and a custom **bow (headshot) critical hit rule**.

---

## Game Overview

Two players secretly deploy their naval fleets onto a grid (default $10 \times 10$). Players take turns calling out coordinates to strike enemy waters. The first player to sink all enemy warships wins.

### The Bow (Headshot) Rule
In traditional Battleship, every single segment of a ship must be struck to sink it. 

In this version:
- Each ship has an identifiable **bow (head)** at its placement origin.
- **Direct hit on the bow**: Triggers an instant critical hit?the ship's health drops immediately to 0 and the **entire ship sinks in one shot**, regardless of its size.
- **Hit on any other segment**: Deals standard damage (reduces ship HP by 1).

---

## Fleet Composition

The game includes 5 historic naval warship classes:

| Symbol | Ship Name | Class | Length |
| :---: | :--- | :--- | :---: |
| **S** | **Shinano** | Aircraft Carrier | **5** |
| **Y** | **Yamato** | Heavy Battleship | **4** |
| **B** | **Belfast** | Light Cruiser | **3** |
| **L** | **Laffey** | Destroyer | **2** |
| **A** | **Albacore** | Submarine | **1** |

The number of ships scales dynamically based on board area ($N \times M$):
- Shinano: $\lfloor \text{area} / 70 \rfloor$
- Yamato: $\lfloor \text{area} / 55 \rfloor$
- Belfast: $\lfloor \text{area} / 40 \rfloor$
- Laffey: $\lfloor \text{area} / 30 \rfloor$
- Albacore: $\lfloor \text{area} / 20 \rfloor$

*(On a standard $10 \times 10$ board, each player commands 12 ships).*

---

## Features

- **Game Modes**:
  - **Single Player (vs. AI)**: Play against a computer opponent using a **Hunt & Target** strategy (searches randomly, then systematically probes adjacent cells upon landing a hit).
  - **Two Players (Pass & Play)**: Local multiplayer with screen-clearing turn transitions to keep fleets hidden.
- **Fleet Deployment**:
  - **Auto-Placement**: Instantly generates legal, non-overlapping ship coordinates.
  - **Manual Placement**: Allows choosing direction (`H` for horizontal, `V` for vertical) and origin coordinates step-by-step.
- **Dual Radar Display**: Side-by-side terminal rendering showing your fleet and the enemy radar (with fog of war).
- **Memory Safety**: Clean dynamic memory allocation with zero memory leaks (verified with Valgrind).

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
| `X` | Red | Hit on a warship |
| `o` | Gray | Missed shot (water splash) |
| `#` | Yellow | Sunk / destroyed ship |

---

## Project Structure

```text
??? src/
?   ??? main.c       # Game loop, menus, and turn transitions
?   ??? board.c/.h   # 2D matrix allocation, cleanup, and rendering
?   ??? ship.c/.h    # Placement validation and fleet generator
?   ??? game.c/.h    # Attack mechanics, bow/headshot rule, and AI logic
?   ??? structs.h    # Shared structures and constants
??? Makefile         # Build configuration
??? README.md
```

---

## Build & Run

### Prerequisites
- GCC compiler supporting C99 (`gcc`)
- Make utility (`make`)

### Compile
```bash
make
```

### Run
```bash
./battleships
```

### Clean
```bash
make clean
```

---

## Memory Check

Tested with Valgrind for memory leaks:
```bash
valgrind --leak-check=full ./battleships
```
Result: `0 errors from 0 contexts`, `0 bytes in use at exit` (clean heap).
