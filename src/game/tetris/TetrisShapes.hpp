#pragma once
#include <array>

// Pure Tetris shape definitions - no external dependencies
// Can be used in any project

// Tetromino types
enum class TetrominoType {
    I, O, T, S, Z, J, L, NONE
};

// Grid dimensions
constexpr int TETRIS_BOARD_WIDTH = 10;
constexpr int TETRIS_BOARD_HEIGHT = 20;

// Each shape is represented as a 4x4 grid
// 1 = filled, 0 = empty
using TetrisShape = std::array<std::array<int, 4>, 4>;

// Shape data for each tetromino type (rotation 0)
const TetrisShape TETRIS_SHAPE_I = {{
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
}};

const TetrisShape TETRIS_SHAPE_O = {{
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
}};

const TetrisShape TETRIS_SHAPE_T = {{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 1, 0},
    {0, 0, 0, 0}
}};

const TetrisShape TETRIS_SHAPE_S = {{
    {0, 0, 0, 0},
    {0, 0, 1, 1},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
}};

const TetrisShape TETRIS_SHAPE_Z = {{
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 0}
}};

const TetrisShape TETRIS_SHAPE_J = {{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 0}
}};

const TetrisShape TETRIS_SHAPE_L = {{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
}};

// Get base shape matrix for a tetromino type
inline TetrisShape getTetrisBaseShape(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return TETRIS_SHAPE_I;
        case TetrominoType::O: return TETRIS_SHAPE_O;
        case TetrominoType::T: return TETRIS_SHAPE_T;
        case TetrominoType::S: return TETRIS_SHAPE_S;
        case TetrominoType::Z: return TETRIS_SHAPE_Z;
        case TetrominoType::J: return TETRIS_SHAPE_J;
        case TetrominoType::L: return TETRIS_SHAPE_L;
        default: return TetrisShape{};
    }
}

// Rotate a shape matrix 90 degrees clockwise
inline TetrisShape rotateTetrisShape(const TetrisShape& shape) {
    TetrisShape rotated{};
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            rotated[x][3 - y] = shape[y][x];
        }
    }
    return rotated;
}
