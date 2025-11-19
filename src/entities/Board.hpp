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
    RectangleShape blockBackground;  // Solid color background for vibrant colors
    RectangleShape borderShape;
    Vector2f boardPosition;

    // Store texture index for each cell (-1 = no texture assigned)
    array<array<int, BOARD_WIDTH>, BOARD_HEIGHT> textureIndices;

public:
    Board(TetrisBoard* board)
        : tetrisBoard(board),
          blockShape(),
          borderShape(),
          boardPosition() {
        // Initialize all texture indices to -1 (unassigned)
        for (auto& row : this->textureIndices) {
            row.fill(-1);
        }
    }

    void onCreate() override {
        // Position board in center-left of screen
        this->boardPosition = Vector2f(50.0f, 50.0f);

        // Setup block background (solid color for vibrant display)
        this->blockBackground.setSize(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        this->blockBackground.setOutlineThickness(1.0f);
        this->blockBackground.setOutlineColor(Color(0, 0, 0));

        // Setup block shape template (transparent textured layer)
        this->blockShape.setSize(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        this->blockShape.setOutlineThickness(0.0f);

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

        // Draw placed blocks with persistent textures
        if (this->tetrisBoard) {
            auto& assetManager = AssetManager::getInstance();
            const auto& textureNames = assetManager.getTextureNames();
            const auto& grid = this->tetrisBoard->getGrid();

            for (auto y = 0; y < BOARD_HEIGHT; y++) {
                for (auto x = 0; x < BOARD_WIDTH; x++) {
                    if (grid[y][x] == 0)
                        continue;

                    auto posX = this->boardPosition.x + x * BLOCK_SIZE;
                    auto posY = this->boardPosition.y + y * BLOCK_SIZE;

                    auto cellColor = this->getColorFromIndex(grid[y][x]);

                    // Draw solid color background first for vibrant colors
                    this->blockBackground.setPosition(posX, posY);
                    this->blockBackground.setFillColor(cellColor);
                    window.draw(this->blockBackground);

                    // Use stored texture index for this cell
                    if (this->textureIndices[y][x] >= 0 && !textureNames.empty()) {
                        auto textureIdx = static_cast<size_t>(this->textureIndices[y][x]);
                        auto textureName = textureNames[textureIdx % textureNames.size()];
                        auto texture = assetManager.getTexture(textureName);

                        if (texture) {
                            // Texture is loaded - draw semi-transparent layer on top
                            this->blockShape.setPosition(posX, posY);
                            this->blockShape.setTexture(texture.get());
                            auto transparentColor = Color(255, 255, 255, 180);
                            this->blockShape.setFillColor(transparentColor);
                            window.draw(this->blockShape);
                        }
                    }
                }
            }
        }
    }

    auto getBoardPosition() const { return this->boardPosition; }

    // Set texture index for a specific cell (called when piece locks)
    void setTextureForCell(int x, int y, int textureIndex) {
        if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
            this->textureIndices[y][x] = textureIndex;
        }
    }

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
            case 6: return Color(80, 150, 255); // J (Bright Blue)
            case 7: return Color(255, 220, 50); // L (Bright Orange)
            default: return Color::White;
        }
    }
};
