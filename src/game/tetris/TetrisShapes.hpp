#pragma once
#include <array>
#include <optional>

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

// Pivot point - optional to support "no rotation" pieces
// nullopt = no rotation (O-piece)
// {x, y} = integer pivot point in 4×4 grid
using Pivot = optional<pair<int, int>>;

// Tetromino data - groups shape with its rotation behavior
struct TetrominoData {
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

// Tetromino definitions - complete data for each piece type
const TetrominoData I_PIECE = {
    .shape = TETRIS_SHAPE_I,
    .pivot = {{2, 1}}
};

const TetrominoData O_PIECE = {
    .shape = TETRIS_SHAPE_O,
    .pivot = nullopt
};

const TetrominoData T_PIECE = {
    .shape = TETRIS_SHAPE_T,
    .pivot = {{2, 1}}
};

const TetrominoData S_PIECE = {
    .shape = TETRIS_SHAPE_S,
    .pivot = {{2, 2}}
};

const TetrominoData Z_PIECE = {
    .shape = TETRIS_SHAPE_Z,
    .pivot = {{2, 2}}
};

const TetrominoData J_PIECE = {
    .shape = TETRIS_SHAPE_J,
    .pivot = {{2, 1}}
};

const TetrominoData L_PIECE = {
    .shape = TETRIS_SHAPE_L,
    .pivot = {{2, 1}}
};

// Get tetromino data (shape + pivot) for a tetromino type
inline TetrominoData getTetromino(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return I_PIECE;
        case TetrominoType::O: return O_PIECE;
        case TetrominoType::T: return T_PIECE;
        case TetrominoType::S: return S_PIECE;
        case TetrominoType::Z: return Z_PIECE;
        case TetrominoType::J: return J_PIECE;
        case TetrominoType::L: return L_PIECE;
        default: return {TetrisShape{}, nullopt};
    }
}

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
// Uses integer pivot points for cleaner rotation
// Based on Tetris rotation conventions
inline Pivot getTetrisRotationCenter(TetrominoType type) {
    switch (type) {
        case TetrominoType::I: return {{2, 1}}; // Approximate center of I-piece
        case TetrominoType::O: return nullopt;   // O-piece doesn't rotate
        case TetrominoType::T: return {{2, 1}}; // Center top block of T
        case TetrominoType::S: return {{2, 2}}; // Center of mass
        case TetrominoType::Z: return {{2, 2}}; // Center of mass
        case TetrominoType::J: return {{2, 1}}; // Corner where J bends
        case TetrominoType::L: return {{2, 1}}; // Corner where L bends
        default: return nullopt;
    }
}

// Rotate a shape matrix 90 degrees clockwise around its rotation center
// Returns the original shape if center is nullopt (no rotation)
auto rotateTetrisShape(const TetrisShape& shape, const Pivot& center) {
    // No rotation for pieces without a center (O-piece)
    if (!center.has_value()) {
        return shape;
    }

    auto rotated = TetrisShape{};
    auto [px, py] = center.value();

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

// Convenience overload that uses the default rotation center for the piece type
auto rotateTetrisShape(const TetrisShape& shape, TetrominoType type) {
    return rotateTetrisShape(shape, getTetrisRotationCenter(type));
}
