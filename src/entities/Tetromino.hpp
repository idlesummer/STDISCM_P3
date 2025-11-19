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

    // Store single texture index for this piece (all cells share the same texture)
    int pieceTextureIndex;
    inline static size_t nextTextureIndex = 0; // Global counter for unique texture per piece

public:
    Tetromino(const TetrisPiece* piece, Board* board)
        : tetrisPiece(piece),
          board(board),
          color(piece ? getTetrominoColor(piece->getType()) : Color::White),
          blockShape(),
          boardPosition(),
          pieceTextureIndex(static_cast<int>(nextTextureIndex++)) {
    }

    void onCreate() override {
        this->boardPosition = this->board->getBoardPosition();

        // Setup block background (solid color for vibrant display)
        this->blockBackground.setSize(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        this->blockBackground.setOutlineThickness(1.0f);
        this->blockBackground.setOutlineColor(Color(0, 0, 0));

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
        auto gridX = this->tetrisPiece->getX();
        auto gridY = this->tetrisPiece->getY();

        // Draw ghost piece (shadow) first - without texture or borders
        auto ghostY = this->tetrisPiece->calculateGhostY();
        if (ghostY != gridY) {  // Only draw if ghost is below current position
            auto ghostColor = Color(100, 100, 100, 100);  // Semi-transparent grey

            // Set ghost rendering properties (no borders)
            this->blockBackground.setOutlineThickness(0.0f);
            this->blockBackground.setFillColor(ghostColor);

            for (auto y = 0; y < 4; y++) {
                for (auto x = 0; x < 4; x++) {
                    if (shape[y][x] == 0) continue;

                    auto px = this->boardPosition.x + (gridX + x) * BLOCK_SIZE;
                    auto py = this->boardPosition.y + (ghostY + y) * BLOCK_SIZE;
                    this->blockBackground.setPosition(px, py);
                    window.draw(this->blockBackground);
                }
            }
        }

        // Set actual piece rendering properties (with borders)
        this->blockBackground.setOutlineThickness(1.0f);
        this->blockBackground.setOutlineColor(Color(0, 0, 0));

        // Draw actual tetromino piece with progressive texture loading
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

                // Use piece texture index for all filled cells
                if (this->pieceTextureIndex < 0 || textureNames.empty())
                    continue;

                auto textureName = textureNames[static_cast<size_t>(this->pieceTextureIndex) % textureNames.size()];
                auto texture = assetManager.getTexture(textureName);
                if (!texture)
                    continue;

                // Texture is loaded - draw semi-transparent layer on top
                this->blockShape.setPosition(posX, posY);
                this->blockShape.setTexture(texture.get());
                auto transparentColor = Color(255, 255, 255, 230);
                this->blockShape.setFillColor(transparentColor);
                window.draw(this->blockShape);
            }
        }
    }

    // Update the piece being rendered (for when engine spawns new piece)
    void setPiece(const TetrisPiece* piece) {
        this->tetrisPiece = piece;
        if (piece) {
            this->color = getTetrominoColor(piece->getType());
            this->pieceTextureIndex = static_cast<int>(nextTextureIndex++); // Assign new texture for new piece
        }
    }

    // Get texture index for a specific cell (for transferring to board on lock)
    auto getTextureIndexForCell(int x, int y) const -> int {
        if (!this->tetrisPiece || x < 0 || x >= 4 || y < 0 || y >= 4)
            return -1;

        // Return piece texture index if this cell is filled in the current shape
        const auto& shape = this->tetrisPiece->getShape();
        return (shape[y][x] != 0) ? this->pieceTextureIndex : -1;
    }
};
