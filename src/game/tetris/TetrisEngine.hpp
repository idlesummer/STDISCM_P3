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
    optional<TetrisPiece> activePiece;
    char nextPieceType;
    char heldPieceType;
    bool canSwapHold;
    bool gameOver;

public:
    TetrisEngine()
        : board(),
          activePiece(nullopt),
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
        this->activePiece = nullopt;
        this->nextPieceType = '\0';
        this->heldPieceType = '\0';
        this->canSwapHold = true;
        this->gameOver = false;
    }

    // Movement methods - return true if successful
    auto moveLeft() -> bool {
        return (activePiece && !gameOver)
            ? activePiece->moveLeft()
            : false;
    }

    auto moveRight() -> bool {
        return (activePiece && !gameOver)
            ? activePiece->moveRight()
            : false;
    }

    auto rotate() -> bool {
        return (activePiece && !gameOver)
            ? activePiece->rotate()
            : false;
    }

    auto softDrop() -> bool {
        return (activePiece && !gameOver)
            ? activePiece->moveDown()
            : false;
    }

    auto hardDrop() -> int {
        return (activePiece && !gameOver)
            ? activePiece->hardDrop()
            : 0;
    }

    // Hold system - swap current piece with held piece
    auto hold() -> bool {
        if (!this->activePiece || this->gameOver || !this->canSwapHold)
            return false;

        auto currentType = this->activePiece->getType();
        this->activePiece = nullopt;

        if (this->heldPieceType == '\0') {      // First time holding
            this->heldPieceType = currentType;  // Store current
            this->spawnNextPiece();             // Spawn next
        
        } else {                                // Swap current with held piece
            char temp = this->heldPieceType;
            this->heldPieceType = currentType;
            this->spawnPiece(temp);
        }

        this->canSwapHold = false;              // Lock hold until next piece
        return true;
    }

    // Lock current piece and spawn next
    // Returns number of lines cleared (0 if no lines cleared)
    auto lockCurrentPiece() -> int {
        if (!this->activePiece)
            return 0;

        // Place piece on board
        this->activePiece->placeOnBoard();
        this->activePiece = nullopt;

        // Clear completed lines
        int linesCleared = this->board.clearLines();

        // Check for game over
        if (this->board.isTopRowOccupied()) {
            this->gameOver = true;
            return linesCleared;
        }
        
        this->canSwapHold = true;   // Reset hold ability
        this->spawnNextPiece();     // Spawn next piece
        return linesCleared;
    }

    // State queries
    auto getActivePiece() const -> const TetrisPiece* {
        return this->activePiece.has_value() ? &this->activePiece.value() : nullptr;
    }

    auto getNextPieceType() const -> char {
        return this->nextPieceType;
    }

    auto getHeldPieceType() const -> char {
        return this->heldPieceType;
    }

    auto canHold() const -> bool {
        return this->canSwapHold && this->activePiece.has_value() && !this->gameOver;
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
        this->activePiece = TetrisPiece(type, &this->board);

        // Check if piece can spawn (game over check)
        if (!this->activePiece->canSpawn()) {
            this->gameOver = true;
            this->activePiece = nullopt;
        }
    }

    void spawnNextPiece() {
        auto typeToSpawn = this->nextPieceType;
        this->nextPieceType = TetrominoType::random();
        this->spawnPiece(typeToSpawn);
    }
};
