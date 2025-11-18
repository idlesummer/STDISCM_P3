#pragma once
#include "TetrisShapes.hpp"
#include "TetrisBoard.hpp"

// Pure Tetris piece logic - no rendering or external dependencies
// Handles piece state, movement, rotation, and collision detection

class TetrisPiece {
private:
    TetrominoType type;
    TetrisShape currentShape;
    int gridX;
    int gridY;
    TetrisBoard* board; // Non-owning pointer to the game board

public:
    TetrisPiece(TetrominoType type, TetrisBoard* board, int startX = 3, int startY = 0)
        : type(type),
          board(board),
          gridX(startX),
          gridY(startY),
          currentShape(getTetrisBaseShape(type)) {
    }

    // Movement methods - return true if successful, false if blocked
    bool moveLeft() {
        if (this->board && this->board->isValidPosition(this->currentShape, this->gridX - 1, this->gridY)) {
            this->gridX--;
            return true;
        }
        return false;
    }

    bool moveRight() {
        if (this->board && this->board->isValidPosition(this->currentShape, this->gridX + 1, this->gridY)) {
            this->gridX++;
            return true;
        }
        return false;
    }

    bool moveDown() {
        if (this->board && this->board->isValidPosition(this->currentShape, this->gridX, this->gridY + 1)) {
            this->gridY++;
            return true;
        }
        return false;
    }

    // Rotate clockwise with wall kick support
    bool rotate() {
        TetrisShape rotated = rotateTetrisShape(this->currentShape);

        if (!this->board)
            return false;

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

    // Hard drop - move down until collision, return number of rows dropped
    int hardDrop() {
        int rowsDropped = 0;
        while (this->moveDown()) {
            rowsDropped++;
        }
        return rowsDropped;
    }

    // Place this piece on the board
    void placeOnBoard() {
        if (this->board) {
            this->board->placePiece(this->currentShape, this->gridX, this->gridY, this->type);
        }
    }

    // Check if piece can be placed at current position (spawn check)
    bool canSpawn() const {
        if (!this->board)
            return false;
        return this->board->isValidPosition(this->currentShape, this->gridX, this->gridY);
    }

    // Getters
    TetrominoType getType() const {
        return this->type;
    }

    const TetrisShape& getShape() const {
        return this->currentShape;
    }

    int getX() const {
        return this->gridX;
    }

    int getY() const {
        return this->gridY;
    }

    // Set position (useful for testing or repositioning)
    void setPosition(int x, int y) {
        this->gridX = x;
        this->gridY = y;
    }
};
