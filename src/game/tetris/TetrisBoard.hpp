#pragma once
#include "TetrisShapes.hpp"
#include <array>
#include <algorithm>

using namespace std;


class TetrisBoard {
private:
    // Board grid: 0 = empty, 1-7 = color index for each piece type
    array<array<int, TETRIS_BOARD_WIDTH>, TETRIS_BOARD_HEIGHT> grid;
    int totalLinesCleared;

public:
    TetrisBoard()
        : grid{},
          totalLinesCleared(0) {

        this->reset();
    }

    // Reset the board to empty state
    void reset() {
        for (auto& row : this->grid)
            row.fill(0);
        this->totalLinesCleared = 0;
    }

    // Check if a position is valid (within bounds and not occupied)
    auto isValidPosition(const TetrisShape& shape, int gridX, int gridY) const {
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                auto boardX = gridX + x;
                auto boardY = gridY + y;

                // Check bounds
                if (!this->isInBounds(boardX, boardY))
                    return false;

                // Check if cell is occupied
                if (this->isOccupied(boardX, boardY))
                    return false;
            }
        }
        return true;
    }

    // Place a tetromino on the board
    void placePiece(const TetrisShape& shape, int gridX, int gridY, char type) {
        // Map piece type to color index (1-7)
        int colorIndex = 0;
        switch (type) {
            case 'I': colorIndex = 1; break;
            case 'O': colorIndex = 2; break;
            case 'T': colorIndex = 3; break;
            case 'S': colorIndex = 4; break;
            case 'Z': colorIndex = 5; break;
            case 'J': colorIndex = 6; break;
            case 'L': colorIndex = 7; break;
            default: colorIndex = 0; break;
        }

        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                auto boardX = gridX + x;
                auto boardY = gridY + y;

                if (this->isInBounds(boardX, boardY))
                    this->grid[boardY][boardX] = colorIndex;
            }
        }
    }

    // Check and clear completed lines, return number of lines cleared
    auto clearLines() {
        auto cleared = 0;

        for (auto y = TETRIS_BOARD_HEIGHT - 1; y >= 0;) {
            if (!this->isRowComplete(y))    // Skip incomplete rows
                y--; continue;

            this->clearRow(y);              // Handle complete row
            cleared++;
        }

        return cleared;
    }

    // Check if the top row has any blocks (game over condition)
    auto isTopRowOccupied() const {
        return ranges::any_of(this->grid[0], [](auto cell) { return cell != 0; });
    }

    // Get cell value at position
    auto getCell(int x, int y) const {
        return this->isInBounds(x, y) ? this->grid[y][x] : -1;
    }

    // Get the entire grid (for rendering or serialization)
    const auto& getGrid() const {
        return this->grid;
    }

    auto getTotalLinesCleared() const {
        return this->totalLinesCleared;
    }

    // Bounds checking helpers (public for external use)
    auto isInBounds(int x, int y) const -> bool {
        return x >= 0 && x < TETRIS_BOARD_WIDTH &&
               y >= 0 && y < TETRIS_BOARD_HEIGHT;
    }

    // Check if a cell is occupied (non-zero value)
    auto isOccupied(int x, int y) const -> bool {
        return this->isInBounds(x, y) && this->grid[y][x] != 0;
    }

private:
    // Check if a row is completely filled
    auto isRowComplete(int y) const -> bool {
        for (auto x = 0; x < TETRIS_BOARD_WIDTH; x++) {
            if (!this->isOccupied(x, y))
                return false;
        }
        return true;
    }

    // Clear a row by shifting all rows above it down
    void clearRow(int y) {
        this->totalLinesCleared++;

        // Shift all rows above down
        for (auto shiftY = y; shiftY > 0; shiftY--)
            for (auto x = 0; x < TETRIS_BOARD_WIDTH; x++)
                this->grid[shiftY][x] = this->grid[shiftY - 1][x];

        // Clear top row
        for (auto x = 0; x < TETRIS_BOARD_WIDTH; x++)
            this->grid[0][x] = 0;
    }
};
