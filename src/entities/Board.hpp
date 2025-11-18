#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>
#include <array>

using namespace std;
using namespace sf;
using namespace Tetris;

class Board : public Entity {
private:
    // Board grid: 0 = empty, 1-7 = color index
    array<array<int, BOARD_WIDTH>, BOARD_HEIGHT> grid;

    // Visual representation
    RectangleShape blockShape;
    RectangleShape borderShape;

    Vector2f boardPosition;
    int linesCleared;

public:
    Board() : linesCleared(0) {
        // Initialize empty grid
        for (auto& row : grid) {
            row.fill(0);
        }
    }

    void onCreate() override {
        // Position board in center-left of screen
        boardPosition = Vector2f(50.0f, 50.0f);

        // Setup block shape template
        blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        blockShape.setOutlineThickness(1.0f);
        blockShape.setOutlineColor(Color(50, 50, 50));

        // Setup border
        borderShape.setSize(Vector2f(BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE));
        borderShape.setPosition(boardPosition);
        borderShape.setFillColor(Color::Transparent);
        borderShape.setOutlineThickness(2.0f);
        borderShape.setOutlineColor(Color::White);
    }

    void onDraw(RenderWindow& window) override {
        // Draw border
        window.draw(borderShape);

        // Draw placed blocks
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                if (grid[y][x] != 0) {
                    blockShape.setPosition(
                        boardPosition.x + x * BLOCK_SIZE,
                        boardPosition.y + y * BLOCK_SIZE
                    );
                    blockShape.setFillColor(getColorFromIndex(grid[y][x]));
                    window.draw(blockShape);
                }
            }
        }
    }

    // Check if a position is valid (within bounds and not occupied)
    bool isValidPosition(const ShapeMatrix& shape, int gridX, int gridY) const {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x] != 0) {
                    int boardX = gridX + x;
                    int boardY = gridY + y;

                    // Check bounds
                    if (boardX < 0 || boardX >= BOARD_WIDTH ||
                        boardY < 0 || boardY >= BOARD_HEIGHT) {
                        return false;
                    }

                    // Check collision with placed blocks
                    if (grid[boardY][boardX] != 0) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // Place a tetromino on the board
    void placeTetromino(const ShapeMatrix& shape, int gridX, int gridY, TetrominoType type) {
        int colorIndex = static_cast<int>(type) + 1;

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (shape[y][x] != 0) {
                    int boardX = gridX + x;
                    int boardY = gridY + y;
                    if (boardX >= 0 && boardX < BOARD_WIDTH &&
                        boardY >= 0 && boardY < BOARD_HEIGHT) {
                        grid[boardY][boardX] = colorIndex;
                    }
                }
            }
        }
    }

    // Check and clear completed lines, return number of lines cleared
    int clearLines() {
        int cleared = 0;

        for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
            bool isComplete = true;
            for (int x = 0; x < BOARD_WIDTH; x++) {
                if (grid[y][x] == 0) {
                    isComplete = false;
                    break;
                }
            }

            if (isComplete) {
                cleared++;
                linesCleared++;

                // Shift all rows above down
                for (int shiftY = y; shiftY > 0; shiftY--) {
                    for (int x = 0; x < BOARD_WIDTH; x++) {
                        grid[shiftY][x] = grid[shiftY - 1][x];
                    }
                }

                // Clear top row
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    grid[0][x] = 0;
                }

                // Check same row again since we shifted
                y++;
            }
        }

        return cleared;
    }

    // Check if the top row has any blocks (game over condition)
    bool isTopRowOccupied() const {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (grid[0][x] != 0) {
                return true;
            }
        }
        return false;
    }

    Vector2f getBoardPosition() const { return boardPosition; }
    int getTotalLinesCleared() const { return linesCleared; }

    void reset() {
        for (auto& row : grid) {
            row.fill(0);
        }
        linesCleared = 0;
    }

private:
    Color getColorFromIndex(int index) const {
        switch (index) {
            case 1: return Color::Cyan;      // I
            case 2: return Color::Yellow;    // O
            case 3: return Color::Magenta;   // T
            case 4: return Color::Green;     // S
            case 5: return Color::Red;       // Z
            case 6: return Color::Blue;      // J
            case 7: return Color(255, 165, 0); // L (Orange)
            default: return Color::White;
        }
    }
};
