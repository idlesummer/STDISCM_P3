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

// Rotation center for each piece (x, y coordinates in the 4x4 grid)
// These define the pivot point around which the piece rotates
struct RotationCenter {
    float x;
    float y;
};

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

// Get rotation center for each piece type
// Based on Super Rotation System (SRS) used in modern Tetris
inline RotationCenter getTetrisRotationCenter(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return {1.5f, 1.5f}; // Between blocks
        case TetrominoType::O: return {1.5f, 1.5f}; // Center (doesn't rotate visibly)
        case TetrominoType::T: return {1.0f, 1.0f}; // Center block
        case TetrominoType::S: return {1.0f, 1.0f}; // Center of mass
        case TetrominoType::Z: return {1.0f, 1.0f}; // Center of mass
        case TetrominoType::J: return {1.0f, 1.0f}; // Corner of L
        case TetrominoType::L: return {1.0f, 1.0f}; // Corner of L
        default: return {1.5f, 1.5f};
    }
}

// Rotate a shape matrix 90 degrees clockwise around its rotation center
auto rotateTetrisShape(const TetrisShape& shape, const RotationCenter& center) {
    auto rotated = TetrisShape{};

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x] == 0)
                continue;

            // Translate to center
            float px = x - center.x;
            float py = y - center.y;

            // Rotate 90 degrees clockwise: (x, y) -> (y, -x)
            float rx = py;
            float ry = -px;

            // Translate back
            int newX = static_cast<int>(rx + center.x + 0.5f);
            int newY = static_cast<int>(ry + center.y + 0.5f);

            // Bounds check
            if (newX >= 0 && newX < 4 && newY >= 0 && newY < 4) {
                rotated[newY][newX] = shape[y][x];
            }
        }
    }

    return rotated;
}

// Convenience overload that uses the default rotation center for the piece type
auto rotateTetrisShape(const TetrisShape& shape, TetrominoType type) {
    return rotateTetrisShape(shape, getTetrisRotationCenter(type));
}
