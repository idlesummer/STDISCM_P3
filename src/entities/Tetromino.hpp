#pragma once
#include "../core/Entity.hpp"
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

public:
    Tetromino(const TetrisPiece* piece, Board* board)
        : tetrisPiece(piece),
          board(board),
          color(piece ? getTetrominoColor(piece->getType()) : Color::White),
          blockShape(),
          boardPosition() {
    }

    void onCreate() override {
        this->boardPosition = this->board->getBoardPosition();

        // Setup block rendering
        this->blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        this->blockShape.setFillColor(this->color);
        this->blockShape.setOutlineThickness(1.0f);
        this->blockShape.setOutlineColor(Color(50, 50, 50));
    }

    void onDraw(RenderWindow& window) override {
        if (!this->tetrisPiece)
            return;

        const auto& shape = this->tetrisPiece->getShape();
        int gridX = this->tetrisPiece->getX();
        int gridY = this->tetrisPiece->getY();

        // Draw ghost piece (shadow) first
        int ghostY = this->tetrisPiece->calculateGhostY();
        if (ghostY != gridY) {  // Only draw if ghost is below current position
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

            // Restore original colors for actual piece
            this->blockShape.setFillColor(this->color);
            this->blockShape.setOutlineColor(Color(50, 50, 50));
        }

        // Draw actual tetromino piece
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (shape[y][x] == 0)
                    continue;

                this->blockShape.setPosition(
                    this->boardPosition.x + (gridX + x) * BLOCK_SIZE,
                    this->boardPosition.y + (gridY + y) * BLOCK_SIZE
                );
                window.draw(this->blockShape);
            }
        }
    }

    // Update the piece being rendered (for when engine spawns new piece)
    void setPiece(const TetrisPiece* piece) {
        this->tetrisPiece = piece;
        if (piece)
            this->color = getTetrominoColor(piece->getType());
    }
};
