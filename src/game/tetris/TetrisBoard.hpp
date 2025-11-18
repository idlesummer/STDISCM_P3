#pragma once
#include "TetrisShapes.hpp"
#include <array>
#include <algorithm>

using namespace std;

// Pure Tetris board logic - no rendering or external dependencies
// Can be used in any project

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
        for (auto& row : this->grid) {
            row.fill(0);
        }
        this->totalLinesCleared = 0;
    }

    // Check if a position is valid (within bounds and not occupied)
    bool isValidPosition(const TetrisShape& shape, int gridX, int gridY) const {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                int boardX = gridX + x;
                int boardY = gridY + y;

                // Check bounds
                if (!this->isInBounds(boardX, boardY)) {
                    return false;
                }

                // Check if cell is occupied
                if (this->isOccupied(boardX, boardY)) {
                    return false;
                }
            }
        }
        return true;
    }

    // Place a tetromino on the board
    void placePiece(const TetrisShape& shape, int gridX, int gridY, TetrominoType type) {
        int colorIndex = static_cast<int>(type) + 1;

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                int boardX = gridX + x;
                int boardY = gridY + y;

                if (this->isInBounds(boardX, boardY)) {
                    this->grid[boardY][boardX] = colorIndex;
                }
            }
        }
    }

    // Check and clear completed lines, return number of lines cleared
    int clearLines() {
        int cleared = 0;

        for (int y = TETRIS_BOARD_HEIGHT - 1; y >= 0; y--) {
            bool isComplete = true;

            for (int x = 0; x < TETRIS_BOARD_WIDTH; x++) {
                if (!this->isOccupied(x, y)) {
                    isComplete = false;
                    break;
                }
            }

            if (!isComplete)
                continue;

            cleared++;
            this->totalLinesCleared++;

            // Shift all rows above down
            for (int shiftY = y; shiftY > 0; shiftY--) {
                for (int x = 0; x < TETRIS_BOARD_WIDTH; x++)
                    this->grid[shiftY][x] = this->grid[shiftY - 1][x];
            }

            // Clear top row
            for (int x = 0; x < TETRIS_BOARD_WIDTH; x++) 
                this->grid[0][x] = 0;

            // Check same row again since we shifted
            y++;
        }

        return cleared;
    }

    // Check if the top row has any blocks (game over condition)
    bool isTopRowOccupied() const {
        return any_of(this->grid[0].begin(), this->grid[0].end(),
                          [](int cell) { return cell != 0; });
    }

    // Get cell value at position
    int getCell(int x, int y) const {
        if (!this->isInBounds(x, y)) {
            return -1; // Out of bounds
        }
        return this->grid[y][x];
    }

    // Get the entire grid (for rendering or serialization)
    const auto& getGrid() const {
        return this->grid;
    }

    int getTotalLinesCleared() const {
        return this->totalLinesCleared;
    }

    int getWidth() const {
        return TETRIS_BOARD_WIDTH;
    }

    int getHeight() const {
        return TETRIS_BOARD_HEIGHT;
    }

private:
    // Helper: Check if coordinates are within board bounds
    bool isInBounds(int x, int y) const {
        return x >= 0 && x < TETRIS_BOARD_WIDTH &&
               y >= 0 && y < TETRIS_BOARD_HEIGHT;
    }

    // Helper: Check if a cell is occupied (non-zero value)
    bool isOccupied(int x, int y) const {
        return this->isInBounds(x, y) && this->grid[y][x] != 0;
    }
};
