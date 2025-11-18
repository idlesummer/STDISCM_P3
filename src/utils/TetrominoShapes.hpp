#pragma once
#include <array>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace Tetris {

// Tetromino types
enum class TetrominoType {
    I, O, T, S, Z, J, L, NONE
};

// Grid dimensions
constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
constexpr int BLOCK_SIZE = 30;

// Each shape is represented as a 4x4 grid
// 1 = filled, 0 = empty
using ShapeMatrix = array<array<int, 4>, 4>;

// Shape data for each tetromino type (rotation 0)
const ShapeMatrix SHAPE_I = {{
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
}};

const ShapeMatrix SHAPE_O = {{
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
}};

const ShapeMatrix SHAPE_T = {{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 1, 0},
    {0, 0, 0, 0}
}};

const ShapeMatrix SHAPE_S = {{
    {0, 0, 0, 0},
    {0, 0, 1, 1},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
}};

const ShapeMatrix SHAPE_Z = {{
    {0, 0, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 0}
}};

const ShapeMatrix SHAPE_J = {{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 0}
}};

const ShapeMatrix SHAPE_L = {{
    {0, 0, 0, 0},
    {0, 1, 1, 1},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
}};

// Color mapping for each tetromino type
inline Color getTetrominoColor(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return Color::Cyan;
        case TetrominoType::O: return Color::Yellow;
        case TetrominoType::T: return Color::Magenta;
        case TetrominoType::S: return Color::Green;
        case TetrominoType::Z: return Color::Red;
        case TetrominoType::J: return Color::Blue;
        case TetrominoType::L: return Color(255, 165, 0); // Orange
        default: return Color::White;
    }
}

// Get base shape matrix for a tetromino type
inline ShapeMatrix getBaseShape(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return SHAPE_I;
        case TetrominoType::O: return SHAPE_O;
        case TetrominoType::T: return SHAPE_T;
        case TetrominoType::S: return SHAPE_S;
        case TetrominoType::Z: return SHAPE_Z;
        case TetrominoType::J: return SHAPE_J;
        case TetrominoType::L: return SHAPE_L;
        default: return ShapeMatrix{};
    }
}

// Rotate a shape matrix 90 degrees clockwise
inline ShapeMatrix rotateShape(const ShapeMatrix& shape) {
    ShapeMatrix rotated{};
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            rotated[x][3 - y] = shape[y][x];
        }
    }
    return rotated;
}

} // namespace Tetris
