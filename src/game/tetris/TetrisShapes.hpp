#pragma once
#include <array>
#include <optional>
#include <random>
#include <string_view>

using namespace std;


// Grid dimensions
constexpr auto TETRIS_BOARD_WIDTH = 10;
constexpr auto TETRIS_BOARD_HEIGHT = 20;

using TetrisShape = array<array<int, 4>, 4>;
using Pivot = optional<pair<int, int>>;

struct TetrominoData {
    char type;
    Pivot pivot;
    TetrisShape shape;

    // Check if coordinates are within the 4x4 shape bounds
    auto isInBounds(int x, int y) const -> bool {
        return x >= 0 && x < 4 && y >= 0 && y < 4;
    }

    // Rotate this tetromino 90 degrees clockwise
    auto rotate() const {
        auto rotated = TetrisShape();

        // nullopt pivot means rotate entire 4x4 matrix (for 3x3 shapes centered at 1,1)
        if (!pivot.has_value()) {
            for (auto y = 0; y < 4; y++)
                for (auto x = 0; x < 4; x++) // Rotate entire 4x4 matrix: (x, y) to (3-y, x)
                    rotated[x][3 - y] = shape[y][x];
            return rotated;
        }

        // Standard pivot-based rotation for I and O pieces
        auto [px, py] = pivot.value();
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                // Translate to pivot
                auto dx = x - px;
                auto dy = y - py;

                // Rotate 90 degrees clockwise: (x, y) -> (-y, x)
                // (In screen coordinates where y increases downward)
                auto rx = -dy;
                auto ry = dx;

                // Translate back
                auto newX = rx + px;
                auto newY = ry + py;

                // Bounds check
                if (this->isInBounds(newX, newY))
                    rotated[newY][newX] = shape[y][x];
            }
        }

        return rotated;
    }
};

// Tetromino definitions - complete data for each piece type
const auto I_PIECE = TetrominoData{
    .type = 'I',
    .pivot = nullopt,
    .shape = {{
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const auto O_PIECE = TetrominoData{
    .type = 'O',
    .pivot = nullopt,
    .shape = {{
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }}
};

const auto T_PIECE = TetrominoData{
    .type = 'T',
    .pivot = {{1, 1}},  // 3x3 shape centered at (1,1)
    .shape = {{
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const auto S_PIECE = TetrominoData{
    .type = 'S',
    .pivot = {{1, 1}},  // 3x3 shape centered at (1,1)
    .shape = {{
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const auto Z_PIECE = TetrominoData{
    .type = 'Z',
    .pivot = {{1, 1}},  // 3x3 shape centered at (1,1)
    .shape = {{
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const auto J_PIECE = TetrominoData{
    .type = 'J',
    .pivot = {{1, 1}},  // 3x3 shape centered at (1,1)
    .shape = {{
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

const auto L_PIECE = TetrominoData{
    .type = 'L',
    .pivot = {{1, 1}},  // 3x3 shape centered at (1,1)
    .shape = {{
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

// Get tetromino data for a piece type
auto getTetromino(char type) -> TetrominoData {
    switch (type) {
        default: return {'\0', nullopt, TetrisShape{}};
        case 'I': return I_PIECE;
        case 'O': return O_PIECE;
        case 'T': return T_PIECE;
        case 'S': return S_PIECE;
        case 'Z': return Z_PIECE;
        case 'J': return J_PIECE;
        case 'L': return L_PIECE;
    }
}

// Static class for tetromino type registry and factory methods
class TetrominoType {
public:
    // Registry of all tetromino types
    static constexpr array<char, 7> ALL_TYPES = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
    static constexpr int TYPE_COUNT = 7;

    // Factory method: Get tetromino data for a piece type
    static auto getData(char type) -> TetrominoData {
        return getTetromino(type);
    }

    // Get human-readable name for a piece type
    static auto getName(char type) -> string_view {
        switch (type) {
            default:  return "Unknown";
            case 'I': return "I-Piece (Line)";
            case 'O': return "O-Piece (Square)";
            case 'T': return "T-Piece";
            case 'S': return "S-Piece";
            case 'Z': return "Z-Piece";
            case 'J': return "J-Piece";
            case 'L': return "L-Piece";
        }
    }

    // Get random tetromino type
    static auto random() -> char {
        static mt19937 rng{random_device{}()};
        static uniform_int_distribution<int> dist(0, TYPE_COUNT - 1);
        return ALL_TYPES[dist(rng)];
    }

    // Get random tetromino type (with custom RNG for testing)
    template<typename RNG>
    static auto random(RNG& rng) -> char {
        uniform_int_distribution<int> dist(0, TYPE_COUNT - 1);
        return ALL_TYPES[dist(rng)];
    }

    // Check if a type is valid
    static auto isValid(char type) -> bool {
        for (auto t : ALL_TYPES) {
            if (t == type) return true;
        }
        return false;
    }

    // Deleted constructors (static class only)
    TetrominoType() = delete;
    TetrominoType(const TetrominoType&) = delete;
    TetrominoType& operator=(const TetrominoType&) = delete;
};
