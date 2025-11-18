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

        fallInterval = seconds(1.0f); // Start at 1 second per fall
    }

    void onCreate() override {
        // Create board
        board = make_shared<Board>();
        this->addEntity(board);

        // Create UI
        scoreDisplay = make_shared<TetrisScoreText>(Vector2f(400, 50));
        this->addEntity(scoreDisplay);

        nextPreview = make_shared<NextPiecePreview>(Vector2f(400, 150));
        this->addEntity(nextPreview);

        titleText = make_shared<MenuText>("TETRIS", Vector2f(400, 10), 30);
        this->addEntity(titleText);

        controlsText = make_shared<MenuText>(
            "Arrow Keys: Move/Rotate | Space: Hard Drop | ESC: Quit",
            Vector2f(50, 650), 16);
        this->addEntity(controlsText);

        // Generate first pieces
        nextPieceType = getRandomPieceType();
        nextPreview->setNextPiece(nextPieceType);

        // Spawn first piece
        spawnNewPiece();
    }

    void onInput(Event& event) override {
        Scene::onInput(event);

        if (isGameOver) {
            // Restart on Enter
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                restartGame();
            }
            return;
        }

        if (event.type == Event::KeyPressed) {
            if (!activePiece) return;

            switch (event.key.code) {
                case Keyboard::Left:
                    activePiece->moveLeft();
                    break;

                case Keyboard::Right:
                    activePiece->moveRight();
                    break;

                case Keyboard::Down:
                    activePiece->moveDown();
                    fallTimer = Time::Zero; // Reset fall timer
                    break;

                case Keyboard::Up:
                    activePiece->rotate();
                    break;

                case Keyboard::Space:
                    activePiece->hardDrop();
                    lockPiece();
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

        if (isGameOver || !activePiece) return;

        // Update fall timer
        fallTimer += dt;

        // Adjust fall speed based on level
        Time currentFallInterval = seconds(max(0.1f, 1.0f - (scoreDisplay->getLevel() - 1) * 0.1f));

        if (fallTimer >= currentFallInterval) {
            fallTimer = Time::Zero;

            // Try to move piece down
            if (!activePiece->moveDown()) {
                // Piece can't move down - lock it
                lockPiece();
            }
        }
    }

    void onDraw(RenderWindow& window) override {
        // Draw all entities (board, pieces, UI)
        this->drawEntities(window);
    }

private:
    TetrominoType getRandomPieceType() {
        int random = pieceDistribution(rng);
        return static_cast<TetrominoType>(random);
    }

    void spawnNewPiece() {
        // Use the next piece
        TetrominoType typeToSpawn = nextPieceType;

        // Generate new next piece
        nextPieceType = getRandomPieceType();
        nextPreview->setNextPiece(nextPieceType);

        // Create the new piece
        activePiece = make_shared<Tetromino>(typeToSpawn, board.get());
        this->addEntity(activePiece);

        // Check if piece can spawn (game over check)
        if (!activePiece->canSpawn()) {
            triggerGameOver();
        }
    }

    void lockPiece() {
        if (!activePiece) return;

        // Place piece on board
        activePiece->placeOnBoard();

        // Remove active piece
        this->removeEntity(activePiece);
        activePiece = nullptr;

        // Clear completed lines
        int linesCleared = board->clearLines();
        if (linesCleared > 0) {
            scoreDisplay->addLines(linesCleared);
        }

        // Check for game over (top row occupied)
        if (board->isTopRowOccupied()) {
            triggerGameOver();
            return;
        }

        // Spawn next piece
        spawnNewPiece();
    }

    void triggerGameOver() {
        isGameOver = true;

        // Show game over text
        gameOverText = make_shared<MenuText>("GAME OVER", Vector2f(250, 300), 40);
        this->addEntity(gameOverText);

        auto scoreText = make_shared<MenuText>(
            "Final Score: " + to_string(scoreDisplay->getScore()),
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
        isGameOver = false;
        isPaused = false;
        fallTimer = Time::Zero;
        activePiece = nullptr;

        // Recreate everything
        onCreate();
    }
};
