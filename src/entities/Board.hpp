#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>
#include <array>

using namespace std;
using namespace sf;
using namespace Tetris;

class Board : public Entity {
public:
    Board() 
        : blockShape(),
          borderShape(),
          boardPosition(),
          linesCleared(0) {

        for (auto& row : grid)  
            row.fill(0);
    }

    void onCreate() override {
        // Position board in center-left of screen
        this->boardPosition = Vector2f(50.0f, 50.0f);

        // Setup block shape template
        blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        blockShape.setOutlineThickness(1.0f);
        blockShape.setOutlineColor(Color(50, 50, 50));

        // Setup border
        borderShape.setSize(Vector2f(BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE));
        borderShape.setPosition(this->boardPosition);
        borderShape.setFillColor(Color::Transparent);
        borderShape.setOutlineThickness(2.0f);
        borderShape.setOutlineColor(Color::White);
    }

    void onDraw(RenderWindow& window) override {
        // Draw border
        window.draw(this->borderShape);

        // Draw grid lines (light grey)
        auto gridColor = Color(40, 40, 40);  // Light grey
        auto gridLines = VertexArray(Lines);

        // Vertical lines
        for (auto x = 0; x <= BOARD_WIDTH; x++) {
            auto xPos = this->boardPosition.x + x * BLOCK_SIZE;
            gridLines.append(Vertex(Vector2f(xPos, this->boardPosition.y), gridColor));
            gridLines.append(Vertex(Vector2f(xPos, this->boardPosition.y + BOARD_HEIGHT * BLOCK_SIZE), gridColor));
        }

        // Horizontal lines
        for (auto y = 0; y <= BOARD_HEIGHT; y++) {
            auto yPos = boardPosition.y + y * BLOCK_SIZE;
            gridLines.append(Vertex(Vector2f(boardPosition.x, yPos), gridColor));
            gridLines.append(Vertex(Vector2f(boardPosition.x + BOARD_WIDTH * BLOCK_SIZE, yPos), gridColor));
        }

        window.draw(gridLines);

        // Draw placed blocks
        for (auto y = 0; y < BOARD_HEIGHT; y++) {
            for (auto x = 0; x < BOARD_WIDTH; x++) {
                if (grid[y][x] == 0)
                    continue;
                auto posX = this->boardPosition.x + x * BLOCK_SIZE;
                auto posY = this->boardPosition.y + y * BLOCK_SIZE;
                this->blockShape.setPosition(posX, posY);
                this->blockShape.setFillColor(this->getColorFromIndex(grid[y][x]));
                window.draw(blockShape);
            }
        }
    }

    // Check if a position is valid (within bounds and not occupied)
    auto isValidPosition(const ShapeMatrix& shape, int gridX, int gridY) const {
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                auto boardX = gridX + x;
                auto boardY = gridY + y;

                if (!(boardX >= 0 && boardX < BOARD_WIDTH && boardY > 0 && boardY < BOARD_HEIGHT)) 
                    return false;

                if (grid[boardY][boardX] != 0) 
                    return false;
            }
        }
        return true;
    }

    // Place a tetromino on the board
    void placeTetromino(const ShapeMatrix& shape, int gridX, int gridY, TetrominoType type) {
        auto colorIndex = static_cast<int>(type) + 1;

        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                auto boardX = gridX + x;
                auto boardY = gridY + y;
                if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 && boardY < BOARD_HEIGHT)
                    grid[boardY][boardX] = colorIndex;
            }
        }
    }

    // Check and clear completed lines, return number of lines cleared
    auto clearLines() {
        auto cleared = 0;

        for (auto y = BOARD_HEIGHT - 1; y >= 0; y--) {
            auto isComplete = true;

            for (auto x = 0; x < BOARD_WIDTH; x++) {
                if (grid[y][x] != 0) continue;
                isComplete = false;
                break;
            }

            if (!isComplete)
                break;

            cleared++;
            linesCleared++;

            // Shift all rows above down
            for (auto shiftY = y; shiftY > 0; shiftY--)
                for (auto x = 0; x < BOARD_WIDTH; x++)
                    grid[shiftY][x] = grid[shiftY - 1][x];

            // Clear top row
            for (auto x = 0; x < BOARD_WIDTH; x++)
                grid[0][x] = 0;

            // Check same row again since we shifted
            y++;
        }

        return cleared;
    }

    // Check if the top row has any blocks (game over condition)
    bool isTopRowOccupied() const {
        return ranges::any_of(this->grid[0], [](auto cell) {
            return cell != 0;
        });
    }
    
    void reset() {
        for (auto& row : grid)
            row.fill(0);
        linesCleared = 0;
    }
    
    auto getBoardPosition() const { return boardPosition; }
    auto getTotalLinesCleared() const { return linesCleared; }

private:
    auto getColorFromIndex(int index) const -> Color {
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

    // Board grid: 0 = empty, 1-7 = color index
    array<array<int, BOARD_WIDTH>, BOARD_HEIGHT> grid;
    RectangleShape blockShape;
    RectangleShape borderShape;
    Vector2f boardPosition;
    int linesCleared;
};
