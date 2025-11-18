#pragma once
#include <array>

using namespace std;


// Tetromino types
enum class TetrominoType {
    I, O, T, S, Z, J, L, NONE
};

// Grid dimensions
constexpr auto TETRIS_BOARD_WIDTH = 10;
constexpr auto TETRIS_BOARD_HEIGHT = 20;

// Each shape is represented as a 4x4 grid
// 1 = filled, 0 = empty
using TetrisShape = array<array<int, 4>, 4>;

// Shape data for each tetromino type (rotation 0)
const auto TETRIS_SHAPE_I = TetrisShape{{
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
}};

const auto TETRIS_SHAPE_O = TetrisShape{{
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
}};

const auto TETRIS_SHAPE_T = TetrisShape{{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 1, 0},
    {0, 0, 0, 0}
}};

const auto TETRIS_SHAPE_S = TetrisShape{{
    {0, 0, 0, 0},
    {0, 0, 1, 1},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
}};

const auto TETRIS_SHAPE_Z = TetrisShape{{
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 0}
}};

const auto TETRIS_SHAPE_J = TetrisShape{{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 0}
}};

const auto TETRIS_SHAPE_L = TetrisShape{{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
}};

// Get base shape matrix for a tetromino type
auto getTetrisBaseShape(TetrominoType type) -> TetrisShape {
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
auto rotateTetrisShape(const TetrisShape& shape) {
    auto rotated = TetrisShape{};
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            rotated[x][3 - y] = shape[y][x];
    return rotated;
}
