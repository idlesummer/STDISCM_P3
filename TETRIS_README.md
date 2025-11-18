# Tetris - Minimal Implementation

A simple Tetris game built with the existing game engine architecture using SFML.

## Features

- All 7 standard Tetromino pieces (I, O, T, S, Z, J, L)
- Piece rotation with basic wall kicks
- Line clearing with score tracking
- Progressive difficulty (speed increases with level)
- Next piece preview
- Game over detection
- Restart functionality

## Controls

- **Arrow Left/Right**: Move piece horizontally
- **Arrow Up**: Rotate piece clockwise
- **Arrow Down**: Soft drop (move down faster)
- **Space**: Hard drop (instant drop to bottom)
- **Enter**: Restart game (after game over)
- **ESC**: Exit game

## Scoring

- 1 line: 100 × level points
- 2 lines: 300 × level points
- 3 lines: 500 × level points
- 4 lines (Tetris!): 800 × level points

Level increases every 10 lines cleared, which also increases falling speed.

## Building

### Prerequisites

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libfreetype6-dev \
    libopenal-dev \
    libflac-dev \
    libvorbis-dev
```

### Build Steps

```bash
cd /home/user/STDISCM_P3
cmake -B build -S .
cmake --build build
```

### Run

```bash
./build/bin/STDISCM_P3
```

## Architecture

The implementation follows the existing engine's component-based architecture:

### Files Created

- `src/utils/TetrominoShapes.hpp` - Shape definitions and rotation logic
- `src/entities/Board.hpp` - Game board grid management and collision detection
- `src/entities/Tetromino.hpp` - Active falling piece entity
- `src/entities/TetrisScoreText.hpp` - Score/lines/level display
- `src/entities/NextPiecePreview.hpp` - Next piece preview display
- `src/scenes/TetrisScene.hpp` - Main game scene orchestrating gameplay

### Design Patterns

- **Entity Component System**: Each game object is an Entity with lifecycle hooks
- **Scene Management**: Clean scene transitions and state management
- **Fixed Timestep Loop**: Consistent game physics at 60 updates/second
- **Gravity System**: Progressive falling speed based on player level

## Code Structure

```
TetrisScene (Scene)
├── Board (Entity)           - Manages 10×20 grid, collision, line clearing
├── Tetromino (Entity)        - Active falling piece with movement/rotation
├── TetrisScoreText (Entity)  - Displays score, lines, level
├── NextPiecePreview (Entity) - Shows upcoming piece
└── MenuText (Entities)       - Title and controls text
```

## Implementation Notes

- Pieces spawn at the top center of the board
- Collision detection checks grid boundaries and placed blocks
- Wall kicks allow rotation near walls (basic implementation)
- Game over occurs when a new piece cannot spawn
- Random piece generation uses C++20's `<random>` library
- Fall speed formula: `max(0.1s, 1.0s - (level - 1) × 0.1s)`
