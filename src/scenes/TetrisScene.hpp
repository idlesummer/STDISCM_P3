#pragma once
#include "../core/Scene.hpp"
#include "../entities/Board.hpp"
#include "../entities/Tetromino.hpp"
#include "../entities/TetrisScoreText.hpp"
#include "../entities/NextPiecePreview.hpp"
#include "../entities/HoldPiecePreview.hpp"
#include "../entities/MenuText.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

using namespace std;
using namespace sf;
using namespace Tetris;

class TetrisScene : public Scene {
private:
    shared_ptr<Board> board;
    shared_ptr<Tetromino> activePiece;
    shared_ptr<TetrisScoreText> scoreDisplay;
    shared_ptr<NextPiecePreview> nextPreview;
    shared_ptr<HoldPiecePreview> holdPreview;
    shared_ptr<MenuText> titleText;
    shared_ptr<MenuText> gameOverText;
    shared_ptr<MenuText> controlsText;

    // Game state
    bool isGameOver;
    bool isPaused;

    // Gravity timer
    Time fallTimer;
    Time fallInterval;

    char nextPieceType;

    // Hold system
    char heldPieceType;
    bool canSwapHold;

public:
    TetrisScene()
        : board(),
          activePiece(),
          scoreDisplay(),
          nextPreview(),
          holdPreview(),
          titleText(),
          gameOverText(),
          controlsText(),
          isGameOver(false),
          isPaused(false),
          fallTimer(Time::Zero),
          fallInterval(seconds(1.0f)),
          nextPieceType('\0'),
          heldPieceType('\0'),
          canSwapHold(true) {
    }

    void onCreate() override {
        // Create board
        this->board = make_shared<Board>();
        this->addEntity(this->board);

        // Create UI
        this->scoreDisplay = make_shared<TetrisScoreText>(Vector2f(400, 50));
        this->addEntity(this->scoreDisplay);

        this->nextPreview = make_shared<NextPiecePreview>(Vector2f(400, 150));
        this->addEntity(this->nextPreview);

        this->holdPreview = make_shared<HoldPiecePreview>(Vector2f(400, 320));
        this->addEntity(this->holdPreview);

        this->titleText = make_shared<MenuText>("TETRIS", Vector2f(400, 10), 30);
        this->addEntity(this->titleText);

        this->controlsText = make_shared<MenuText>(
            "Arrow Keys/WASD: Move/Rotate | Space: Hard Drop | Shift: Hold | ESC: Quit",
            Vector2f(50, 650), 16);
        this->addEntity(this->controlsText);

        // Generate first pieces
        this->nextPieceType = this->getRandomPieceType();
        this->nextPreview->setNextPiece(this->nextPieceType);

        // Spawn first piece
        this->spawnNewPiece();
    }

    void onInput(Event& event) override {
        if (this->isGameOver) { // Restart on Enter
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
                this->restartGame();
            return;
        }

        if (event.type == Event::KeyPressed) {
            if (!this->activePiece) return;

            switch (event.key.code) {
                default: break;

                case Keyboard::Left:
                    this->activePiece->moveLeft();
                    break;

                case Keyboard::Right:
                    this->activePiece->moveRight();
                    break;

                case Keyboard::Down:
                    this->activePiece->moveDown();
                    this->fallTimer = Time::Zero; // Reset fall timer
                    break;

                case Keyboard::Up:
                    this->activePiece->rotate();
                    break;

                case Keyboard::Space:
                    this->activePiece->hardDrop();
                    this->lockPiece();
                    break;

                case Keyboard::LShift:
                    this->holdPiece();
                    break;

                case Keyboard::Escape:  // TODO: Add pause or quit functionality
                    break;
            }
        }
    }

    void onUpdate(Time dt) override {
        // Update all entities (animations, etc.)
        this->updateEntities(dt);

        if (this->isGameOver || !this->activePiece) 
            return;

        // Update fall timer
        this->fallTimer += dt;
        if (this->fallTimer < this->fallInterval)
            return;

        this->fallTimer = Time::Zero;

        // Try to move piece down
        if (!this->activePiece->moveDown())
            this->lockPiece();  // Piece can't move down - lock it
    }

    // Draw all entities (board, pieces, UI)
    void onDraw(RenderWindow& window) override {
        this->drawEntities(window);
    }

private:
    auto getRandomPieceType() {
        return TetrominoType::random();
    }

    void spawnNewPiece() {
        // Use the next piece
        auto typeToSpawn = this->nextPieceType;

        // Generate new next piece
        this->nextPieceType = this->getRandomPieceType();
        this->nextPreview->setNextPiece(this->nextPieceType);

        // Create the new piece
        this->activePiece = make_shared<Tetromino>(typeToSpawn, this->board.get());
        this->addEntity(this->activePiece);

        // Check if piece can spawn (game over check)
        if (!this->activePiece->canSpawn())
            this->triggerGameOver();
    }

    void holdPiece() {
        // Can't hold if no active piece or hold is locked
        if (!this->activePiece || !this->canSwapHold)
            return;

        // Get current piece type
        auto currentType = this->activePiece->getType();

        // Remove current piece from entities
        this->removeEntity(this->activePiece);
        this->activePiece = nullptr;

        if (this->heldPieceType == '\0') {
            // First time holding - store current, spawn next
            this->heldPieceType = currentType;
            this->holdPreview->setHeldPiece(this->heldPieceType);
            this->spawnNewPiece();

        } else {
            // Swap current with held piece
            char temp = this->heldPieceType;
            this->heldPieceType = currentType;
            this->holdPreview->setHeldPiece(this->heldPieceType);

            // Spawn the previously held piece
            this->activePiece = make_shared<Tetromino>(temp, this->board.get());
            this->addEntity(this->activePiece);

            // Check if piece can spawn (game over check)
            if (!this->activePiece->canSpawn())
                this->triggerGameOver();
        }

        // Lock hold until piece is placed
        this->canSwapHold = false;
        this->holdPreview->setLocked(true);

        // Reset fall timer
        this->fallTimer = Time::Zero;
    }

    void lockPiece() {
        if (!this->activePiece) return;

        // Place piece on board
        this->activePiece->placeOnBoard();

        // Remove active piece
        this->removeEntity(this->activePiece);
        this->activePiece = nullptr;

        // Clear completed lines
        int linesCleared = this->board->clearLines();
        if (linesCleared > 0)
            this->scoreDisplay->addLines(linesCleared);

        // Check for game over (top row occupied)
        if (this->board->isTopRowOccupied()) {
            this->triggerGameOver();
            return;
        }

        // Reset hold ability for next piece
        this->canSwapHold = true;
        this->holdPreview->setLocked(false);

        // Spawn next piece
        this->spawnNewPiece();
    }

    void triggerGameOver() {
        this->isGameOver = true;

        // Show game over text
        this->gameOverText = make_shared<MenuText>("GAME OVER", Vector2f(250, 300), 40);
        this->addEntity(this->gameOverText);

        auto scoreText = make_shared<MenuText>(
            "Final Lines: " + to_string(this->scoreDisplay->getLines()),
            Vector2f(250, 360), 24);
        this->addEntity(scoreText);

        auto restartText = make_shared<MenuText>(
            "Press ENTER to restart",
            Vector2f(230, 420), 20);
        this->addEntity(restartText);
    }

    void restartGame() {
        // Clear all entities
        this->clearEntities();

        // Reset game state
        this->isGameOver = false;
        this->isPaused = false;
        this->fallTimer = Time::Zero;
        this->activePiece = nullptr;

        // Reset hold state
        this->heldPieceType = '\0';
        this->canSwapHold = true;

        // Recreate everything
        this->onCreate();
    }
};
