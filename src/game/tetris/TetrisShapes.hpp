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

// Pivot point - optional to support "no rotation" pieces
// nullopt = no rotation (O-piece)
// {x, y} = integer pivot point in 4×4 grid
using Pivot = optional<pair<int, int>>;

// Tetromino data - groups shape with its rotation behavior
struct TetrominoData {
    char type;
    TetrisShape shape;
    Pivot pivot;

    // Rotate this tetromino 90 degrees clockwise
    TetrisShape rotate() const {
        // No rotation for pieces without a pivot (O-piece)
        if (!pivot.has_value()) {
            return shape;
        }

        auto rotated = TetrisShape{};
        auto [px, py] = pivot.value();

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                // Translate to pivot
                int dx = x - px;
                int dy = y - py;

                // Rotate 90 degrees clockwise: (x, y) -> (y, -x)
                int rx = dy;
                int ry = -dx;

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
    .shape = {{
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }},
    .pivot = {{2, 1}}
};

const TetrominoData O_PIECE = {
    .type = 'O',
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }},
    .pivot = nullopt
};

const TetrominoData T_PIECE = {
    .type = 'T',
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    }},
    .pivot = {{2, 1}}
};

const TetrominoData S_PIECE = {
    .type = 'S',
    .shape = {{
        {0, 0, 0, 0},
        {0, 0, 1, 1},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }},
    .pivot = {{2, 2}}
};

const TetrominoData Z_PIECE = {
    .type = 'Z',
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 0}
    }},
    .pivot = {{2, 2}}
};

const TetrominoData J_PIECE = {
    .type = 'J',
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 1},
        {0, 0, 0, 0}
    }},
    .pivot = {{2, 1}}
};

const TetrominoData L_PIECE = {
    .type = 'L',
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 1},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    }},
    .pivot = {{2, 1}}
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
        default: return {'\0', TetrisShape{}, nullopt};
    }
}
