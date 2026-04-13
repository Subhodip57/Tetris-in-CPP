# 🎮 Terminal Tetris

A fully-featured Tetris game that runs in your terminal, written in **C++**.  
No graphics libraries — just your terminal, ANSI colors, and good old game logic.

```
|          |   NEXT:
|          |   [][][][] 
|    []    |   
|   [][][]  |   SCORE: 300
|          |   LEVEL: 2
|  [][] [] |   LINES: 8
|  [][][]  |
--------------------
A/D: Move  W: Rotate  S: Soft Drop  Space: Hard Drop  Q: Quit
```

---

## ✨ Features

- 🟦 **7 Tetris pieces** — I, J, L, O, S, T, Z with correct shapes
- 🎨 **ANSI colored pieces** — each piece has its own color
- 👻 **Ghost piece** — shows where the piece will land
- 📦 **Next piece preview** — see what's coming
- 🧮 **Scoring system** — classic Tetris scoring (single/double/triple/Tetris)
- ⚡ **Levels** — speed increases every 10 lines cleared
- 🎲 **7-bag randomizer** — fair piece distribution, no long droughts
- ⬇️ **Hard drop** — instant drop with spacebar

---

## 🕹️ Controls

| Key | Action |
|-----|--------|
| `A` | Move left |
| `D` | Move right |
| `S` | Soft drop |
| `W` | Rotate |
| `Space` | Hard drop |
| `Q` | Quit |

---

## 🔨 Build & Run

### Requirements
- Linux / macOS (uses POSIX terminal APIs)
- `g++` with C++11 or later

### Compile
```bash
g++ -std=c++11 -o tetris tetris.cpp
```

### Run
```bash
./tetris
```

> **Note:** Windows is not supported out of the box due to use of `termios.h` and `sys/select.h`.  
> Use WSL (Windows Subsystem for Linux) on Windows.

---

## 🧠 How It Works

### Board
The board is a `20x10` integer array. `0` = empty, `1–7` = locked piece by color ID.

### Pieces
Each piece is defined by 4 `{x, y}` block offsets and a color ID:
```cpp
Piece shapes[7] = {
    {{{0,1},{1,1},{2,1},{3,1}}, 1}, // I - Cyan
    ...
};
```

### 7-Bag Randomizer
Pieces are shuffled in groups of 7, guaranteeing every piece appears once per cycle — just like official Tetris.

### Rotation
Uses a 2D matrix rotation formula:
```
new_x = -old_y
new_y =  old_x
```
Reverts if the rotated position collides.

### Scoring
| Lines Cleared | Points (× Level) |
|---------------|-----------------|
| 1 (Single)    | 100 |
| 2 (Double)    | 300 |
| 3 (Triple)    | 500 |
| 4 (Tetris)    | 800 |

---

## 📁 Project Structure

```
tetris.cpp      # All game logic — single file
README.md       # This file
```

---

## 📌 Possible Improvements

- [ ] High score saving to file
- [ ] Hold piece (`C` key)
- [ ] Wall kick rotation
- [ ] Sound effects (SDL_mixer)
- [ ] Windows support

---

## 📜 License

This project is open source and free to use.
