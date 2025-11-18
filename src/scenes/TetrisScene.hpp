#pragma once
#include "../core/Scene.hpp"
#include "../entities/Board.hpp"
#include "../entities/Tetromino.hpp"
#include "../entities/TetrisScoreText.hpp"
#include "../entities/NextPiecePreview.hpp"
#include "../entities/MenuText.hpp"
#include "../utils/TetrominoShapes.hpp"
#include <SFML/Graphics.hpp>
#include <random>
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
    shared_ptr<MenuText> titleText;
    shared_ptr<MenuText> gameOverText;
    shared_ptr<MenuText> controlsText;

    // Game state
    bool isGameOver;
    bool isPaused;

    // Gravity timer
    Time fallTimer;
    Time fallInterval;

    // Input timing (for key repeat)
    Time moveTimer;
    const Time moveDelay = seconds(0.15f);

    // Random number generation
    mt19937 rng;
    uniform_int_distribution<int> pieceDistribution;

    TetrominoType nextPieceType;

public:
    TetrisScene() : isGameOver(false), isPaused(false),
        fallTimer(Time::Zero), moveTimer(Time::Zero),
        rng(random_device{}()), pieceDistribution(0, 6) {

        this->fallInterval = seconds(1.0f); // Start at 1 second per fall
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

        this->titleText = make_shared<MenuText>("TETRIS", Vector2f(400, 10), 30);
        this->addEntity(this->titleText);

        this->controlsText = make_shared<MenuText>(
            "Arrow Keys/WASD: Move/Rotate | Space: Hard Drop | ESC: Quit",
            Vector2f(50, 650), 16);
        this->addEntity(this->controlsText);

        // Generate first pieces
        this->nextPieceType = this->getRandomPieceType();
        this->nextPreview->setNextPiece(this->nextPieceType);

        // Spawn first piece
        this->spawnNewPiece();
    }

    void onInput(Event& event) override {
        Scene::onInput(event);

        if (this->isGameOver) {
            // Restart on Enter
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                this->restartGame();
            }
            return;
        }

        if (event.type == Event::KeyPressed) {
            if (!this->activePiece) return;

            switch (event.key.code) {
                case Keyboard::Left:
                case Keyboard::A:
                    this->activePiece->moveLeft();
                    break;

                case Keyboard::Right:
                case Keyboard::D:
                    this->activePiece->moveRight();
                    break;

                case Keyboard::Down:
                case Keyboard::S:
                    this->activePiece->moveDown();
                    this->fallTimer = Time::Zero; // Reset fall timer
                    break;

                case Keyboard::Up:
                case Keyboard::W:
                    this->activePiece->rotate();
                    break;

                case Keyboard::Space:
                    this->activePiece->hardDrop();
                    this->lockPiece();
                    break;

                case Keyboard::Escape:
                    // Could add pause or quit functionality
                    break;

                default:
                    break;
            }
        }
    }

    void onUpdate(Time dt) override {
        // Update all entities (animations, etc.)
        this->updateEntities(dt);

        if (this->isGameOver || !this->activePiece) return;

        // Update fall timer
        this->fallTimer += dt;

        // Adjust fall speed based on level
        Time currentFallInterval = seconds(max(0.1f, 1.0f - (this->scoreDisplay->getLevel() - 1) * 0.1f));

        if (this->fallTimer >= currentFallInterval) {
            this->fallTimer = Time::Zero;

            // Try to move piece down
            if (!this->activePiece->moveDown()) {
                // Piece can't move down - lock it
                this->lockPiece();
            }
        }
    }

    void onDraw(RenderWindow& window) override {
        // Draw all entities (board, pieces, UI)
        this->drawEntities(window);
    }

private:
    TetrominoType getRandomPieceType() {
        int random = this->pieceDistribution(this->rng);
        return static_cast<TetrominoType>(random);
    }

    void spawnNewPiece() {
        // Use the next piece
        TetrominoType typeToSpawn = this->nextPieceType;

        // Generate new next piece
        this->nextPieceType = this->getRandomPieceType();
        this->nextPreview->setNextPiece(this->nextPieceType);

        // Create the new piece
        this->activePiece = make_shared<Tetromino>(typeToSpawn, this->board.get());
        this->addEntity(this->activePiece);

        // Check if piece can spawn (game over check)
        if (!this->activePiece->canSpawn()) {
            this->triggerGameOver();
        }
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
        if (linesCleared > 0) {
            this->scoreDisplay->addLines(linesCleared);
        }

        // Check for game over (top row occupied)
        if (this->board->isTopRowOccupied()) {
            this->triggerGameOver();
            return;
        }

        // Spawn next piece
        this->spawnNewPiece();
    }

    void triggerGameOver() {
        this->isGameOver = true;

        // Show game over text
        this->gameOverText = make_shared<MenuText>("GAME OVER", Vector2f(250, 300), 40);
        this->addEntity(this->gameOverText);

        auto scoreText = make_shared<MenuText>(
            "Final Score: " + to_string(this->scoreDisplay->getScore()),
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

        // Recreate everything
        this->onCreate();
    }
};
