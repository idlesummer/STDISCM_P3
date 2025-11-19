#pragma once
#include "TetrisShapes.hpp"
#include "TetrisBoard.hpp"

// Pure Tetris piece logic - no rendering or external dependencies
// Handles piece state, movement, rotation, and collision detection

class TetrisPiece {
private:
    char type;
    TetrisShape shape;
    int gridX;
    int gridY;
    TetrisBoard* board; // Non-owning pointer to the game board

public:
    TetrisPiece(char type, TetrisBoard* board, int startX = 3, int startY = 0)
        : type(type),
          board(board),
          gridX(startX),
          gridY(startY),
          shape(TetrominoType::getData(type).shape) {
    }

    // Movement methods - return true if successful, false if blocked
    auto moveLeft() {
        if (this->board && this->board->isValidPosition(this->shape, this->gridX - 1, this->gridY)) {
            this->gridX--;
            return true;
        }
        return false;
    }

    auto moveRight() {
        if (this->board && this->board->isValidPosition(this->shape, this->gridX + 1, this->gridY)) {
            this->gridX++;
            return true;
        }
        return false;
    }

    auto moveDown() {
        if (this->board && this->board->isValidPosition(this->shape, this->gridX, this->gridY + 1)) {
            this->gridY++;
            return true;
        }
        return false;
    }

    // Rotate clockwise with wall kick support
    auto rotate() {
        auto pieceData = TetrominoType::getData(this->type);
        auto rotatedShape = TetrominoData{this->type, pieceData.pivot, this->shape}.rotate();

        if (!this->board)
            return false;

        // Try basic rotation
        if (this->board->isValidPosition(rotatedShape, this->gridX, this->gridY)) {
            this->shape = rotatedShape;
            return true;
        }

        // Try wall kicks (simple version - just try moving left or right)
        if (this->board->isValidPosition(rotatedShape, this->gridX - 1, this->gridY)) {
            this->shape = rotatedShape;
            this->gridX--;
            return true;
        }

        if (this->board->isValidPosition(rotatedShape, this->gridX + 1, this->gridY)) {
            this->shape = rotatedShape;
            this->gridX++;
            return true;
        }

        return false;
    }

    // Calculate ghost piece Y position (where piece would land if hard dropped)
    auto calculateGhostY() const {
        if (!this->board)
            return this->gridY;

        int ghostY = this->gridY;
        while (this->board->isValidPosition(this->shape, this->gridX, ghostY + 1)) {
            ghostY++;
        }
        return ghostY;
    }

    // Hard drop - move down until collision, return number of rows dropped
    auto hardDrop() {
        int rowsDropped = 0;
        while (this->moveDown())
            rowsDropped++;
        return rowsDropped;
    }

    // Place this piece on the board
    void placeOnBoard() {
        if (this->board)
            this->board->placePiece(this->shape, this->gridX, this->gridY, this->type);
    }

    // Check if piece can be placed at current position (spawn check)
    auto canSpawn() const {
        if (!this->board)
            return false;
        return this->board->isValidPosition(this->shape, this->gridX, this->gridY);
    }

    // Getters
    auto getType() const {
        return this->type;
    }

    const auto& getShape() const {
        return this->shape;
    }

    auto getX() const {
        return this->gridX;
    }

    auto getY() const {
        return this->gridY;
    }

    // Set position (useful for testing or repositioning)
    void setPosition(int x, int y) {
        this->gridX = x;
        this->gridY = y;
    }
};
