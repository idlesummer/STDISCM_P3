#pragma once
#include "TetrisBoard.hpp"
#include "TetrisPiece.hpp"
#include "TetrisShapes.hpp"
#include <optional>

using namespace std;

// Pure game logic coordinator for Tetris
// No SFML dependencies - handles game state, piece spawning, hold system, etc.
class TetrisEngine {
private:
    TetrisBoard board;
    optional<TetrisPiece> currentPiece;
    char nextPieceType;
    char heldPieceType;
    bool canSwapHold;
    bool gameOver;

public:
    TetrisEngine()
        : board(),
          currentPiece(nullopt),
          nextPieceType('\0'),
          heldPieceType('\0'),
          canSwapHold(true),
          gameOver(false) {
    }

    // Initialize game - generates first pieces and spawns
    void start() {
        this->nextPieceType = TetrominoType::random();
        this->spawnPiece(TetrominoType::random());
    }

    // Reset game to initial state
    void reset() {
        this->board.reset();
        this->currentPiece = nullopt;
        this->nextPieceType = '\0';
        this->heldPieceType = '\0';
        this->canSwapHold = true;
        this->gameOver = false;
    }

    // Movement methods - return true if successful
    auto moveLeft() -> bool {
        if (!this->currentPiece || this->gameOver)
            return false;
        return this->currentPiece->moveLeft();
    }

    auto moveRight() -> bool {
        if (!this->currentPiece || this->gameOver)
            return false;
        return this->currentPiece->moveRight();
    }

    auto rotate() -> bool {
        if (!this->currentPiece || this->gameOver)
            return false;
        return this->currentPiece->rotate();
    }

    auto softDrop() -> bool {
        if (!this->currentPiece || this->gameOver)
            return false;
        return this->currentPiece->moveDown();
    }

    auto hardDrop() -> int {
        if (!this->currentPiece || this->gameOver)
            return 0;
        return this->currentPiece->hardDrop();
    }

    // Hold system - swap current piece with held piece
    // Returns true if hold was successful
    auto hold() -> bool {
        if (!this->currentPiece || this->gameOver || !this->canSwapHold)
            return false;

        auto currentType = this->currentPiece->getType();
        this->currentPiece = nullopt;

        if (this->heldPieceType == '\0') {
            // First time holding - store current, spawn next
            this->heldPieceType = currentType;
            this->spawnNextPiece();
        } else {
            // Swap current with held piece
            char temp = this->heldPieceType;
            this->heldPieceType = currentType;
            this->spawnPiece(temp);
        }

        // Lock hold until next piece
        this->canSwapHold = false;
        return true;
    }

    // Lock current piece and spawn next
    // Returns number of lines cleared (0 if no lines cleared)
    auto lockCurrentPiece() -> int {
        if (!this->currentPiece)
            return 0;

        // Place piece on board
        this->currentPiece->placeOnBoard();
        this->currentPiece = nullopt;

        // Clear completed lines
        int linesCleared = this->board.clearLines();

        // Check for game over
        if (this->board.isTopRowOccupied()) {
            this->gameOver = true;
            return linesCleared;
        }

        // Reset hold ability
        this->canSwapHold = true;

        // Spawn next piece
        this->spawnNextPiece();

        return linesCleared;
    }

    // State queries
    auto getCurrentPiece() const -> const TetrisPiece* {
        return this->currentPiece.has_value() ? &this->currentPiece.value() : nullptr;
    }

    auto getNextPieceType() const -> char {
        return this->nextPieceType;
    }

    auto getHeldPieceType() const -> char {
        return this->heldPieceType;
    }

    auto canHold() const -> bool {
        return this->canSwapHold && this->currentPiece.has_value() && !this->gameOver;
    }

    auto isGameOver() const -> bool {
        return this->gameOver;
    }

    const auto& getBoard() const {
        return this->board;
    }

    auto& getBoard() {
        return this->board;
    }

    auto getTotalLinesCleared() const -> int {
        return this->board.getTotalLinesCleared();
    }

private:
    void spawnPiece(char type) {
        this->currentPiece = TetrisPiece(type, &this->board);

        // Check if piece can spawn (game over check)
        if (!this->currentPiece->canSpawn()) {
            this->gameOver = true;
            this->currentPiece = nullopt;
        }
    }

    void spawnNextPiece() {
        auto typeToSpawn = this->nextPieceType;
        this->nextPieceType = TetrominoType::random();
        this->spawnPiece(typeToSpawn);
    }
};
