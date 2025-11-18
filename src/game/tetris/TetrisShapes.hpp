#pragma once
#include <array>
#include <optional>

using namespace std;


// Grid dimensions
constexpr auto TETRIS_BOARD_WIDTH = 10;
constexpr auto TETRIS_BOARD_HEIGHT = 20;

// Each shape is represented as a 4x4 grid
// 1 = filled, 0 = empty
using TetrisShape = array<array<int, 4>, 4>;

// Pivot point - optional to support different grid sizes
// nullopt = 3x3 shape (center pivot implied)
// {x, y} = integer pivot point in 4×4 grid
using Pivot = optional<pair<int, int>>;

// Tetromino data - groups shape with its rotation behavior
struct TetrominoData {
    char type;
    Pivot pivot;
    TetrisShape shape;

    // Rotate this tetromino 90 degrees clockwise
    TetrisShape rotate() const {
        auto rotated = TetrisShape{};

        // nullopt pivot means rotate entire 4x4 matrix (for 3x3 shapes centered at 1,1)
        if (!pivot.has_value()) {
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    // Rotate entire 4x4 matrix: (x, y) -> (3-y, x)
                    rotated[x][3 - y] = shape[y][x];
                }
            }
            return rotated;
        }

        // Standard pivot-based rotation for I and O pieces
        auto [px, py] = pivot.value();
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                // Translate to pivot
                int dx = x - px;
                int dy = y - py;

                // Rotate 90 degrees clockwise: (x, y) -> (-y, x)
                // (In screen coordinates where y increases downward)
                int rx = -dy;
                int ry = dx;

                // Translate back
                int newX = rx + px;
                int newY = ry + py;

                // Bounds check
                if (newX >= 0 && newX < 4 && newY >= 0 && newY < 4) {
                    rotated[newY][newX] = shape[y][x];
                }
            }
        }

        return rotated;
    }
};

// Tetromino definitions - complete data for each piece type
const TetrominoData I_PIECE = {
    .type = 'I',
    .pivot = {{2, 1}},
    .shape = {{
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const TetrominoData O_PIECE = {
    .type = 'O',
    .pivot = {{1, 1}},  // Rotating O doesn't change it, so pivot doesn't matter
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }}
};

const TetrominoData T_PIECE = {
    .type = 'T',
    .pivot = nullopt,  // 3x3 shape centered at (1,1)
    .shape = {{
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const TetrominoData S_PIECE = {
    .type = 'S',
    .pivot = nullopt,  // 3x3 shape centered at (1,1)
    .shape = {{
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const TetrominoData Z_PIECE = {
    .type = 'Z',
    .pivot = nullopt,  // 3x3 shape centered at (1,1)
    .shape = {{
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const TetrominoData J_PIECE = {
    .type = 'J',
    .pivot = nullopt,  // 3x3 shape centered at (1,1)
    .shape = {{
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const TetrominoData L_PIECE = {
    .type = 'L',
    .pivot = nullopt,  // 3x3 shape centered at (1,1)
    .shape = {{
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

// Get tetromino data for a piece type
inline TetrominoData getTetromino(char type) {
    switch (type) {
        case 'I': return I_PIECE;
        case 'O': return O_PIECE;
        case 'T': return T_PIECE;
        case 'S': return S_PIECE;
        case 'Z': return Z_PIECE;
        case 'J': return J_PIECE;
        case 'L': return L_PIECE;
        default: return {'\0', nullopt, TetrisShape{}};
    }
}
