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

        // Setup block rendering
        this->blockShape.setSize(Vector2f(BLOCK_SIZE - 2.0f, BLOCK_SIZE - 2.0f));
        this->blockShape.setFillColor(this->color);
        this->blockShape.setOutlineThickness(2.0f);
        this->blockShape.setOutlineColor(Color(0, 0, 0));
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
            this->blockShape.setTexture(nullptr);
            auto ghostColor = Color(100, 100, 100, 100);  // Semi-transparent grey
            this->blockShape.setFillColor(ghostColor);
            this->blockShape.setOutlineColor(Color(80, 80, 80, 100));

            for (auto y = 0; y < 4; y++) {
                for (auto x = 0; x < 4; x++) {
                    if (shape[y][x] == 0)
                        continue;

                    this->blockShape.setPosition(
                        this->boardPosition.x + (gridX + x) * BLOCK_SIZE,
                        this->boardPosition.y + (ghostY + y) * BLOCK_SIZE
                    );
                    window.draw(this->blockShape);
                }
            }

            // Restore colors for actual piece
            this->blockShape.setOutlineColor(Color(50, 50, 50));
        }

        // Draw actual tetromino piece with progressive texture loading
        size_t cellIndex = 0;
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                this->blockShape.setPosition(
                    this->boardPosition.x + (gridX + x) * BLOCK_SIZE,
                    this->boardPosition.y + (gridY + y) * BLOCK_SIZE
                );

                // Use assigned texture index for this cell
                auto textureIdx = this->cellTextureIndices[y][x];
                if (textureIdx >= 0 && !textureNames.empty()) {
                    auto textureName = textureNames[static_cast<size_t>(textureIdx) % textureNames.size()];
                    auto texture = assetManager.getTexture(textureName);

                    if (texture) {
                        // Texture is loaded - use it with semi-transparent color tint
                        this->blockShape.setTexture(texture.get());
                        auto transparentColor = Color(this->color.r, this->color.g, this->color.b, 180);
                        this->blockShape.setFillColor(transparentColor);
                    } else {
                        // Texture not loaded yet - use solid color fallback
                        this->blockShape.setTexture(nullptr);
                        this->blockShape.setFillColor(this->color);
                    }
                } else {
                    // No texture assigned - use solid color
                    this->blockShape.setTexture(nullptr);
                    this->blockShape.setFillColor(this->color);
                }

                window.draw(this->blockShape);
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
