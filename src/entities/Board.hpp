#pragma once
#include "../core/Entity.hpp"
#include "../core/AssetManager.hpp"
#include "../utils/TetrominoShapes.hpp"
#include "../game/tetris/TetrisBoard.hpp"
#include <SFML/Graphics.hpp>
#include <array>

using namespace std;
using namespace sf;
using namespace Tetris;

class Board : public Entity {
private:
    TetrisBoard* tetrisBoard; // Non-owning pointer to game logic
    RectangleShape blockShape;
    RectangleShape borderShape;
    Vector2f boardPosition;
    size_t textureIndex; // Track which texture to use for next cell

public:
    Board(TetrisBoard* board)
        : tetrisBoard(board),
          blockShape(),
          borderShape(),
          boardPosition(),
          textureIndex(0) {
    }

    void onCreate() override {
        // Position board in center-left of screen
        this->boardPosition = Vector2f(50.0f, 50.0f);

        // Setup block shape template
        this->blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        this->blockShape.setOutlineThickness(1.0f);
        this->blockShape.setOutlineColor(Color(50, 50, 50));

        // Setup border
        this->borderShape.setSize(Vector2f(BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE));
        this->borderShape.setPosition(this->boardPosition);
        this->borderShape.setFillColor(Color::Transparent);
        this->borderShape.setOutlineThickness(2.0f);
        this->borderShape.setOutlineColor(Color::White);
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
            auto yPos = this->boardPosition.y + y * BLOCK_SIZE;
            gridLines.append(Vertex(Vector2f(this->boardPosition.x, yPos), gridColor));
            gridLines.append(Vertex(Vector2f(this->boardPosition.x + BOARD_WIDTH * BLOCK_SIZE, yPos), gridColor));
        }

        window.draw(gridLines);

        // Draw placed blocks with progressive texture loading
        if (this->tetrisBoard) {
            auto& assetManager = AssetManager::getInstance();
            const auto& textureNames = assetManager.getTextureNames();
            const auto& grid = this->tetrisBoard->getGrid();

            size_t cellIndex = 0;
            for (auto y = 0; y < BOARD_HEIGHT; y++) {
                for (auto x = 0; x < BOARD_WIDTH; x++) {
                    if (grid[y][x] == 0)
                        continue;

                    auto posX = this->boardPosition.x + x * BLOCK_SIZE;
                    auto posY = this->boardPosition.y + y * BLOCK_SIZE;
                    this->blockShape.setPosition(posX, posY);

                    // Try to get texture for this cell
                    if (!textureNames.empty()) {
                        auto textureName = textureNames[cellIndex % textureNames.size()];
                        auto texture = assetManager.getTexture(textureName);

                        if (texture) {
                            // Texture is loaded - use it with white fill (no tint)
                            this->blockShape.setTexture(texture.get());
                            this->blockShape.setFillColor(Color::White);
                        } else {
                            // Texture not loaded yet - use solid color fallback
                            this->blockShape.setTexture(nullptr);
                            this->blockShape.setFillColor(this->getColorFromIndex(grid[y][x]));
                        }
                    } else {
                        // No textures queued - use solid color
                        this->blockShape.setTexture(nullptr);
                        this->blockShape.setFillColor(this->getColorFromIndex(grid[y][x]));
                    }

                    window.draw(this->blockShape);
                    cellIndex++;
                }
            }
        }
    }

    auto getBoardPosition() const { return this->boardPosition; }

    // Access to underlying game logic (if needed)
    TetrisBoard* getTetrisBoard() { return this->tetrisBoard; }
    const TetrisBoard* getTetrisBoard() const { return this->tetrisBoard; }

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
};
