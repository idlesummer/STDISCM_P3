#pragma once
#include "../game/tetris/TetrisShapes.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace Tetris {

// Import pure game logic types (project-agnostic)
using TetrisShape = ::TetrisShape;
using ::Pivot;
using ::TetrominoData;

// SFML-specific rendering constants
constexpr int BOARD_WIDTH = TETRIS_BOARD_WIDTH;
constexpr int BOARD_HEIGHT = TETRIS_BOARD_HEIGHT;
constexpr int BLOCK_SIZE = 30;

// Alias for backward compatibility
using ShapeMatrix = TetrisShape;

// SFML-specific: Color mapping for each tetromino type
auto getTetrominoColor(char type) {
    switch (type) {
        case 'I': return Color::Cyan;
        case 'O': return Color::Yellow;
        case 'T': return Color::Magenta;
        case 'S': return Color::Green;
        case 'Z': return Color::Red;
        case 'J': return Color(80, 150, 255); // Bright Blue
        case 'L': return Color(255, 220, 50); // Bright Orange
        default: return Color::White;
    }
}

// Wrapper function for backward compatibility
auto getBaseShape(char type) {
    return TetrominoType::getData(type).shape;
}

} // namespace Tetris
