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

    // Store texture index for the 4x4 shape (same index for all cells of one piece)
    array<array<int, 4>, 4> cellTextureIndices;
    inline static size_t nextTextureIndex = 0; // Global counter for unique texture per piece

public:
    Tetromino(const TetrisPiece* piece, Board* board)
        : tetrisPiece(piece),
          board(board),
          color(piece ? getTetrominoColor(piece->getType()) : Color::White),
          blockShape(),
          boardPosition() {

        // Assign one unique texture for all cells of this piece
        this->assignTextureIndices();
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

            // Remove borders for ghost piece
            this->blockBackground.setOutlineThickness(0.0f);

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

            // Restore borders for actual pieces
            this->blockBackground.setOutlineThickness(1.0f);
        }

        // Draw actual tetromino piece with progressive texture loading
        auto cellIndex = size_t(0);
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
                if (textureIdx < 0 || textureNames.empty())
                    continue;

                auto textureName = textureNames[static_cast<size_t>(textureIdx) % textureNames.size()];
                auto texture = assetManager.getTexture(textureName);
                if (!texture) 
                    continue;

                // Texture is loaded - draw semi-transparent layer on top
                this->blockShape.setPosition(posX, posY);
                this->blockShape.setTexture(texture.get());
                auto transparentColor = Color(255, 255, 255, 100);
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
            this->assignTextureIndices(); // Assign new texture for new piece
        }
    }

    // Get texture index for a specific cell (for transferring to board on lock)
    auto getTextureIndexForCell(int x, int y) const -> int {
        return (x >= 0 && x < 4 && y >= 0 && y < 4)
            ? this->cellTextureIndices[y][x]
            : -1;
    }

private:
    // Assign one texture index for the entire tetromino piece
    // All cells of the same piece share the same texture
    void assignTextureIndices() {
        if (!this->tetrisPiece)
            return;

        // Get one texture index for this entire piece
        auto pieceTextureIndex = static_cast<int>(nextTextureIndex++);
        const auto& shape = this->tetrisPiece->getShape();

        // Initialize all cells to -1 (no texture)
        for (auto y = 0; y < 4; y++)
            for (auto x = 0; x < 4; x++)
                this->cellTextureIndices[y][x] = -1;

        // Assign the same texture index to all filled cells of this piece
        for (auto y = 0; y < 4; y++)
            for (auto x = 0; x < 4; x++)
                if (shape[y][x] != 0)
                    this->cellTextureIndices[y][x] = pieceTextureIndex;
    }
};
