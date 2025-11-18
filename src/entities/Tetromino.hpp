#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include "Board.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;


class Tetromino : public Entity {
private:
    // --- Fields ---
    TetrominoType type;
    ShapeMatrix currentShape;
    Color color;
    int gridX, gridY;           // Position in grid coordinates
    RectangleShape blockShape;
    Board* board;               // Reference to the game board
    Vector2f boardPosition;

public:
    // --- Constructor ---
    Tetromino(TetrominoType type, Board* board)
        : type(type),
          board(board),
          gridX(3),
          gridY(0),
          currentShape(getBaseShape(type)),
          color(getTetrominoColor(type)),
          blockShape(),
          boardPosition() {
    }

    // --- Methods ---
    void onCreate() override {
        this->boardPosition = this->board->getBoardPosition();

        // Setup block rendering
        this->blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        this->blockShape.setFillColor(this->color);
        this->blockShape.setOutlineThickness(1.0f);
        this->blockShape.setOutlineColor(Color(50, 50, 50));
    }

    void onDraw(RenderWindow& window) override {
        // Draw each block of the tetromino
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (this->currentShape[y][x] == 0)
                    continue;

                this->blockShape.setPosition(
                    this->boardPosition.x + (this->gridX + x) * BLOCK_SIZE,
                    this->boardPosition.y + (this->gridY + y) * BLOCK_SIZE
                );
                window.draw(this->blockShape);
            }
        }
    }

    // Movement methods
    auto moveLeft() {
        if (this->board->isValidPosition(this->currentShape, this->gridX - 1, this->gridY)) {
            this->gridX--;
            return true;
        }
        return false;
    }

    auto moveRight() {
        if (this->board->isValidPosition(this->currentShape, this->gridX + 1, this->gridY)) {
            this->gridX++;
            return true;
        }
        return false;
    }

    auto moveDown() {
        if (this->board->isValidPosition(this->currentShape, this->gridX, this->gridY + 1)) {
            this->gridY++;
            return true;
        }
        return false;
    }

    // Rotate clockwise
    auto rotate() {
        auto rotated = rotateShape(this->currentShape);

        // Try basic rotation
        if (this->board->isValidPosition(rotated, this->gridX, this->gridY)) {
            this->currentShape = rotated;
            return true;
        }

        // Try wall kicks (simple version - just try moving left or right)
        if (this->board->isValidPosition(rotated, this->gridX - 1, this->gridY)) {
            this->currentShape = rotated;
            this->gridX--;
            return true;
        }

        if (this->board->isValidPosition(rotated, this->gridX + 1, this->gridY)) {
            this->currentShape = rotated;
            this->gridX++;
            return true;
        }

        return false;
    }

    // Hard drop - move down until collision
    void hardDrop() {
        while (this->moveDown()); // Keep moving down
    }

    // Place this tetromino on the board
    void placeOnBoard() {
        this->board->placeTetromino(this->currentShape, this->gridX, this->gridY, this->type);
    }

    // Check if piece can be placed at starting position
    bool canSpawn() const {
        return this->board->isValidPosition(this->currentShape, this->gridX, this->gridY);
    }

    // Getters
    TetrominoType getType() const { return this->type; }
    ShapeMatrix getShape() const { return this->currentShape; }
    Color getColor() const { return this->color; }
    int getGridX() const { return this->gridX; }
    int getGridY() const { return this->gridY; }
};
