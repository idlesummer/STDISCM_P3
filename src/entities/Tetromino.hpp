#pragma once
#include "../core/Entity.hpp"
#include "../core/AssetManager.hpp"
#include "../utils/TetrominoShapes.hpp"
#include "../game/tetris/TetrisPiece.hpp"
#include "Board.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;


// SFML rendering entity for Tetromino pieces
// Pure renderer - does not own game logic, just renders from TetrisPiece*
class Tetromino : public Entity {
private:
    const TetrisPiece* tetrisPiece;  // Non-owning pointer to game logic
    Color color;
    RectangleShape blockShape;
    RectangleShape blockBackground;  // Solid color background for vibrant colors
    Board* board;                    // Reference to the game board for rendering position
    Vector2f boardPosition;

    // Store texture index for each cell in the 4x4 shape
    array<array<int, 4>, 4> cellTextureIndices;
    static size_t nextTextureIndex; // Global counter for assigning unique textures

public:
    Tetromino(const TetrisPiece* piece, Board* board)
        : tetrisPiece(piece),
          board(board),
          color(piece ? getTetrominoColor(piece->getType()) : Color::White),
          blockShape(),
          boardPosition() {

        // Assign unique texture indices to each cell
        this->assignTextureIndices();
    }

    void onCreate() override {
        this->boardPosition = this->board->getBoardPosition();

        // Setup block background (solid color for vibrant display)
        this->blockBackground.setSize(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        this->blockBackground.setOutlineThickness(0.0f);

        // Setup block rendering (transparent textured layer)
        this->blockShape.setSize(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        this->blockShape.setFillColor(this->color);
        this->blockShape.setOutlineThickness(0.0f);
    }

    void onDraw(RenderWindow& window) override {
        if (!this->tetrisPiece)
            return;

        auto& assetManager = AssetManager::getInstance();
        const auto& textureNames = assetManager.getTextureNames();
        const auto& shape = this->tetrisPiece->getShape();
        int gridX = this->tetrisPiece->getX();
        int gridY = this->tetrisPiece->getY();

        // Draw ghost piece (shadow) first - without texture
        int ghostY = this->tetrisPiece->calculateGhostY();
        if (ghostY != gridY) {  // Only draw if ghost is below current position
            auto ghostColor = Color(100, 100, 100, 100);  // Semi-transparent grey

            for (auto y = 0; y < 4; y++) {
                for (auto x = 0; x < 4; x++) {
                    if (shape[y][x] == 0)
                        continue;

                    this->blockBackground.setPosition(
                        this->boardPosition.x + (gridX + x) * BLOCK_SIZE,
                        this->boardPosition.y + (ghostY + y) * BLOCK_SIZE
                    );
                    this->blockBackground.setFillColor(ghostColor);
                    window.draw(this->blockBackground);
                }
            }
        }

        // Draw actual tetromino piece with progressive texture loading
        size_t cellIndex = 0;
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                auto posX = this->boardPosition.x + (gridX + x) * BLOCK_SIZE;
                auto posY = this->boardPosition.y + (gridY + y) * BLOCK_SIZE;

                // Draw solid color background first for vibrant colors
                this->blockBackground.setPosition(posX, posY);
                this->blockBackground.setFillColor(this->color);
                window.draw(this->blockBackground);

                // Use assigned texture index for this cell
                auto textureIdx = this->cellTextureIndices[y][x];
                if (textureIdx >= 0 && !textureNames.empty()) {
                    auto textureName = textureNames[static_cast<size_t>(textureIdx) % textureNames.size()];
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

    // Update the piece being rendered (for when engine spawns new piece)
    void setPiece(const TetrisPiece* piece) {
        this->tetrisPiece = piece;
        if (piece) {
            this->color = getTetrominoColor(piece->getType());
            this->assignTextureIndices(); // Reassign textures for new piece
        }
    }

    // Get texture index for a specific cell (for transferring to board on lock)
    auto getTextureIndexForCell(int x, int y) const -> int {
        if (x >= 0 && x < 4 && y >= 0 && y < 4) {
            return this->cellTextureIndices[y][x];
        }
        return -1;
    }

private:
    // Assign unique texture indices to each cell of the tetromino
    void assignTextureIndices() {
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                // Assign sequential texture indices for variety
                this->cellTextureIndices[y][x] = static_cast<int>(nextTextureIndex++);
            }
        }
    }
};

// Initialize static member
size_t Tetromino::nextTextureIndex = 0;
