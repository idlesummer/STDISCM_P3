#pragma once
#include "../game/tetris/TetrisShapes.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace Tetris {

// Import pure game logic types (project-agnostic)
using ::TetrominoType;
using TetrisShape = ::TetrisShape;
using ::RotationCenter;

// SFML-specific rendering constants
constexpr int BOARD_WIDTH = TETRIS_BOARD_WIDTH;
constexpr int BOARD_HEIGHT = TETRIS_BOARD_HEIGHT;
constexpr int BLOCK_SIZE = 30;

// Alias for backward compatibility
using ShapeMatrix = TetrisShape;

// Shape constants (aliases to pure logic shapes)
const TetrisShape& SHAPE_I = TETRIS_SHAPE_I;
const TetrisShape& SHAPE_O = TETRIS_SHAPE_O;
const TetrisShape& SHAPE_T = TETRIS_SHAPE_T;
const TetrisShape& SHAPE_S = TETRIS_SHAPE_S;
const TetrisShape& SHAPE_Z = TETRIS_SHAPE_Z;
const TetrisShape& SHAPE_J = TETRIS_SHAPE_J;
const TetrisShape& SHAPE_L = TETRIS_SHAPE_L;

// SFML-specific: Color mapping for each tetromino type
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

// Wrapper functions for backward compatibility
inline TetrisShape getBaseShape(TetrominoType type) {
    return getTetrisBaseShape(type);
}

inline TetrisShape rotateShape(const TetrisShape& shape, TetrominoType type) {
    return rotateTetrisShape(shape, type);
}

inline RotationCenter getRotationCenter(TetrominoType type) {
    return getTetrisRotationCenter(type);
}

} // namespace Tetris
