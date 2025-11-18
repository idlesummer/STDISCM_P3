#pragma once
#include "TetrisBoard.hpp"
#include "TetrisPiece.hpp"
#include "TetrisScoring.hpp"
#include "TetrisShapes.hpp"
#include <memory>
#include <random>
#include <functional>

using namespace std;


enum class TetrisGameState {
    Playing,
    GameOver,
    Paused
};

class TetrisGame {
private:
    TetrisBoard board;
    TetrisScoring scoring;
    unique_ptr<TetrisPiece> activePiece;
    char nextPieceType;
    TetrisGameState state;
    float fallTimer;

    // Random number generation
    mt19937 rng;
    uniform_int_distribution<int> pieceDistribution;

    // Callback for game over event (optional)
    function<void(int finalLines)> onGameOverCallback;

public:
    TetrisGame()
        : board(),
          scoring(),
          activePiece(nullptr),
          nextPieceType('\0'),
          state(TetrisGameState::Playing),
          fallTimer(0.0f),
          rng(random_device{}()),
          pieceDistribution(0, 6),
          onGameOverCallback(nullptr) {
        this->initialize();
    }

    // Initialize/restart the game
    void initialize() {
        this->board.reset();
        this->scoring.reset();
        this->activePiece = nullptr;
        this->state = TetrisGameState::Playing;
        this->fallTimer = 0.0f;

        // Generate first pieces
        this->nextPieceType = this->getRandomPieceType();
        this->spawnNewPiece();
    }

    // Update game state (should be called every frame)
    // deltaTime is in seconds
    void update(float deltaTime) {
        if (this->state != TetrisGameState::Playing || !this->activePiece)
            return;

        // Update fall timer
        this->fallTimer += deltaTime;
        float fallInterval = this->scoring.getFallInterval();

        if (this->fallTimer < fallInterval)
            return;

        this->fallTimer = 0.0f;

        // Try to move piece down
        if (!this->activePiece->moveDown()) 
            this->lockPiece();              // Piece can't move down - lock it
    }

    // Input handling methods - return true if action was successful
    auto moveActiveLeft() {
        if (this->state != TetrisGameState::Playing || !this->activePiece)
            return false;
        return this->activePiece->moveLeft();
    }

    auto moveActiveRight() {
        if (this->state != TetrisGameState::Playing || !this->activePiece)
            return false;
        return this->activePiece->moveRight();
    }

    auto moveActiveDown() {
        if (this->state != TetrisGameState::Playing || !this->activePiece)
            return false;
        bool moved = this->activePiece->moveDown();
        if (moved)
            this->fallTimer = 0.0f; // Reset fall timer
        return moved;
    }

    auto rotateActive() {
        if (this->state != TetrisGameState::Playing || !this->activePiece)
            return false;
        return this->activePiece->rotate();
    }

    void hardDropActive() {
        if (this->state != TetrisGameState::Playing || !this->activePiece)
            return;
        this->activePiece->hardDrop();
        this->lockPiece();
    }

    void pause() {
        if (this->state == TetrisGameState::Playing)
            this->state = TetrisGameState::Paused;
    }

    void resume() {
        if (this->state == TetrisGameState::Paused)
            this->state = TetrisGameState::Playing;
    }

    // Getters for UI
    const TetrisBoard& getBoard() const {
        return this->board;
    }

    const TetrisScoring& getScoring() const {
        return this->scoring;
    }

    const TetrisPiece* getActivePiece() const {
        return this->activePiece.get();
    }

    char getNextPieceType() const {
        return this->nextPieceType;
    }

    TetrisGameState getState() const {
        return this->state;
    }

    bool isGameOver() const {
        return this->state == TetrisGameState::GameOver;
    }

    bool isPaused() const {
        return this->state == TetrisGameState::Paused;
    }

    // Set callback for game over event
    void setGameOverCallback(function<void(int, int)> callback) {
        this->onGameOverCallback = callback;
    }

private:
    auto getRandomPieceType() -> char {
        const char pieces[] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
        int random = this->pieceDistribution(this->rng);
        return pieces[random];
    }

    void spawnNewPiece() {
        // Use the next piece
        auto typeToSpawn = this->nextPieceType;

        // Generate new next piece
        this->nextPieceType = this->getRandomPieceType();

        // Create the new piece
        this->activePiece = make_unique<TetrisPiece>(typeToSpawn, &this->board);

        // Check if piece can spawn (game over check)
        if (!this->activePiece->canSpawn())
            this->triggerGameOver();
    }

    void lockPiece() {
        if (!this->activePiece)
            return;

        // Place piece on board
        this->activePiece->placeOnBoard();
        this->activePiece = nullptr;

        // Clear completed lines
        int linesCleared = this->board.clearLines();
        if (linesCleared > 0)
            this->scoring.addLines(linesCleared);

        // Check for game over (top row occupied)
        if (this->board.isTopRowOccupied()) {
            this->triggerGameOver();
            return;
        }

        // Spawn next piece
        this->spawnNewPiece();
    }

    void triggerGameOver() {
        this->state = TetrisGameState::GameOver;
        this->activePiece = nullptr;

        // Call game over callback if set
        if (this->onGameOverCallback)
            this->onGameOverCallback(this->scoring.getLines());
    }
};
