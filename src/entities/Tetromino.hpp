#pragma once
#include "../core/Entity.hpp"
#include "../utils/TetrominoShapes.hpp"
#include "Board.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
using namespace Tetris;


class Tetromino : public Entity {
public:
    Tetromino(TetrominoType type, Board* board)
        : type(type), 
          board(board), 
          gridX(3), 
          gridY(0) {

        currentShape = getBaseShape(type);
        color = getTetrominoColor(type);
    }

    void onCreate() override {
        boardPosition = board->getBoardPosition();

        // Setup block rendering
        blockShape.setSize(Vector2f(BLOCK_SIZE - 1.0f, BLOCK_SIZE - 1.0f));
        blockShape.setFillColor(color);
        blockShape.setOutlineThickness(1.0f);
        blockShape.setOutlineColor(Color(50, 50, 50));
    }

    void onDraw(RenderWindow& window) override {
        // Draw each block of the tetromino
        for (auto y = 0; y < 4; y++) {
            for (auto x = 0; x < 4; x++) {
                if (currentShape[y][x] == 0)
                    continue;

                blockShape.setPosition(
                    boardPosition.x + (gridX + x) * BLOCK_SIZE,
                    boardPosition.y + (gridY + y) * BLOCK_SIZE
                );
                window.draw(blockShape);
            }
        }
    }

    // Movement methods
    auto moveLeft() {
        if (board->isValidPosition(currentShape, gridX - 1, gridY)) {
            gridX--;
            return true;
        }
        return false;
    }

    auto moveRight() {
        if (board->isValidPosition(currentShape, gridX + 1, gridY)) {
            gridX++;
            return true;
        }
        return false;
    }

    auto moveDown() {
        if (board->isValidPosition(currentShape, gridX, gridY + 1)) {
            gridY++;
            return true;
        }
        return false;
    }

    // Rotate clockwise
    auto rotate() {
        auto rotated = rotateShape(currentShape);

        // Try basic rotation
        if (board->isValidPosition(rotated, gridX, gridY)) {
            currentShape = rotated;
            return true;
        }

        // Try wall kicks (simple version - just try moving left or right)
        if (board->isValidPosition(rotated, gridX - 1, gridY)) {
            currentShape = rotated;
            gridX--;
            return true;
        }

        if (board->isValidPosition(rotated, gridX + 1, gridY)) {
            currentShape = rotated;
            gridX++;
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
        this->board->placeTetromino(currentShape, gridX, gridY, type);
    }

    // Check if piece can be placed at starting position
    bool canSpawn() const {
        return board->isValidPosition(currentShape, gridX, gridY);
    }

    // Getters
    TetrominoType getType() const { return type; }
    ShapeMatrix getShape() const { return currentShape; }
    Color getColor() const { return color; }
    int getGridX() const { return gridX; }
    int getGridY() const { return gridY; }

private:
    TetrominoType type;
    ShapeMatrix currentShape;
    Color color;
    int gridX, gridY;           // Position in grid coordinates
    RectangleShape blockShape;
    Board* board;               // Reference to the game board
    Vector2f boardPosition;
};
