#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include "../game/tetris/TetrisPiece.hpp"
#include "Board.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;


class Tetromino : public Entity {
private:
    TetrisPiece tetrisPiece;    // Pure game logic
    Color color;
    RectangleShape blockShape;
    Board* board;               // Reference to the game board for rendering position
    Vector2f boardPosition;

public:
    Tetromino(char type, Board* board)
        : tetrisPiece(type, &board->getTetrisBoard()),
          board(board),
          color(getTetrominoColor(type)),
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
        const auto& shape = this->tetrisPiece.getShape();
        int gridX = this->tetrisPiece.getX();
        int gridY = this->tetrisPiece.getY();

        // Draw ghost piece (shadow) first
        int ghostY = this->tetrisPiece.calculateGhostY();
        if (ghostY != gridY) {  // Only draw if ghost is below current position
            Color ghostColor(100, 100, 100, 100);  // Semi-transparent grey
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

    // Delegate movement methods to TetrisPiece
    auto moveLeft() {
        return this->tetrisPiece.moveLeft();
    }

    auto moveRight() {
        return this->tetrisPiece.moveRight();
    }

    auto moveDown() {
        return this->tetrisPiece.moveDown();
    }

    auto rotate() {
        return this->tetrisPiece.rotate();
    }

    void hardDrop() {
        this->tetrisPiece.hardDrop();
    }

    void placeOnBoard() {
        this->tetrisPiece.placeOnBoard();
    }

    bool canSpawn() const {
        return this->tetrisPiece.canSpawn();
    }

    // Getters
    char getType() const { return this->tetrisPiece.getType(); }
    ShapeMatrix getShape() const { return this->tetrisPiece.getShape(); }
    Color getColor() const { return this->color; }
    int getGridX() const { return this->tetrisPiece.getX(); }
    int getGridY() const { return this->tetrisPiece.getY(); }

    // Access to underlying game logic (if needed)
    TetrisPiece& getTetrisPiece() { return this->tetrisPiece; }
    const TetrisPiece& getTetrisPiece() const { return this->tetrisPiece; }
};
